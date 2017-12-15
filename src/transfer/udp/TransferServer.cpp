#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <set>
#include "../tcp/Socket.h"
#include "TransferUDPServer.h"
#include <sys/epoll.h>


TransferServer::TransferServer(std::shared_ptr<Socket> socket) : socket(socket), stop_requests(true) {}

TransferServer::~TransferServer() {
    stop();
    join();
}

bool TransferServer::start() {
    if (!stop_requests) return false;
    stop_requests = false;
    thread = std::thread([this] { this->run(); });
    return true;
}

void task(
        std::shared_ptr<Connection> &connection,
        const std::function<bool(id_t, const std::string &)> &handle
) {
    bool close = false;
    auto &&id = connection->id;
    auto &&socket = connection->socket;
    try {
        socket->update(Socket::Event::DATA_IN);
        while (!close && !socket->empty()) {
            auto &&message = socket->receive();
            close = handle(id, message);
        }
    } catch (std::exception &ex) {
        std::cerr << TransferServer::format(id, socket) << " handle error: " << ex.what() << std::endl;
        close = true;
    } catch (...) {
        std::cerr << TransferServer::format(id, socket) << " handle error" << std::endl;
        close = true;
    }
    std::unique_lock<std::mutex> socket_lock(connection->mutex);
    connection->close = close;
    connection->free = true;
    if (connection->close)
        socket->shutdown();
}

void TransferServer::disconnect_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    connections_iterator it = std::begin(connections);
    while (it != std::end(connections))
        it = unsafe_kill(it);
}

void TransferServer::disconnect_unavailable_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    connections_iterator it = std::begin(connections);
    while (it != std::end(connections)) {
        auto &&connection = it->second;
        if (connection->close)
            it = unsafe_kill(it);
        else
            ++it;
    }
}

void remove_descriptor(int epoll_descriptor, epoll_event event) {
    epoll_ctl(epoll_descriptor, EPOLL_CTL_DEL, event.data.fd, &event);
}

void add_descriptor(int epoll_descriptor, socket_t descriptor) {
    epoll_event event{};
    event.events = EPOLLIN | EPOLLHUP;
    event.data.fd = descriptor;
    auto &&ctl_stat = epoll_ctl(epoll_descriptor, EPOLL_CTL_ADD, descriptor, &event);
    if (ctl_stat == -1)
        throw TransferServer::error("descriptor addition failed");
}

void TransferServer::handle_new_connection(int epoll_descriptor, id_t &max_id) {
    auto &&client = socket->accept();
    std::unique_lock<std::mutex> lock(mutex);
    auto &&id = max_id++;
    auto &&address = client->get_address();
    auto &&descriptor = client->get_descriptor();
    auto &&connection_ptr = std::make_shared<Connection>(id, client);
    connections[id] = connection_ptr;
    descriptors[descriptor] = id;
    add_descriptor(epoll_descriptor, descriptor);
    connect_handle(id);
}

void TransferServer::handle_connection(std::shared_ptr<Connection> connection, ThreadPool &pool) {
    std::unique_lock<std::mutex> socket_lock(connection->mutex);
    auto &&socket = connection->socket;
    if (!connection->free) return;
    connection->free = false;
    auto &&handle = [this](id_t id, const std::string &message) -> bool {
        this->handle(id, message);
    };
    pool.enqueue(task, connection, handle);
}

void TransferServer::run() {
    ThreadPool pool(NUM_THREADS);
    id_t max_id = 0;
    int epoll_descriptor = epoll_create(1);
    if (epoll_descriptor == -1)
        throw TransferServer::error("creation of epoll descriptor is impossible");
    add_descriptor(epoll_descriptor, socket->get_descriptor());
    epoll_event events[10];
    while (!stop_requests) {
        auto &&event_cnt = epoll_wait(epoll_descriptor, events, sizeof(events), TIMEOUT_USEC);
        for (auto &&entry: connections) {
            auto &&connection = entry.second;
            connection->socket->update(Socket::TIMEOUT);
        }
        for (auto &&i = 0; i < event_cnt; ++i) {
            auto &&event = events[i];
            auto &&descriptor = (socket_t) event.data.fd;
            if (event.events & EPOLLERR) {
                remove_descriptor(epoll_descriptor, event);
                auto &&connection = connection_by_descriptor(descriptor);
                connection->close = true;
                auto &&id = connection->id;
                auto &&socket = connection->socket;
                std::cerr << TransferServer::format(id, socket) << " epoll error" << std::endl;
            }
            if (event.events & EPOLLHUP) {
                remove_descriptor(epoll_descriptor, event);
                auto &&connection = connection_by_descriptor(descriptor);
                connection->close = true;
            }
            if (event.events & EPOLLIN) {
                if (descriptor == socket->get_descriptor())
                    handle_new_connection(epoll_descriptor, max_id);
                else {
                    auto &&connection = connection_by_descriptor(descriptor);
                    handle_connection(connection, pool);
                }
            }
        }
        disconnect_unavailable_connections();
    }
    disconnect_connections();
    socket->close();
}

void TransferServer::join() {
    if (thread.joinable())
        thread.join();
}

void TransferServer::stop() {
    stop_requests = true;
}

bool TransferServer::stopped() {
    return stop_requests;
}

std::unordered_map<id_t, std::shared_ptr<address_t>> TransferServer::get_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    std::unordered_map<id_t, std::shared_ptr<address_t>> view;
    for (auto &&entry: connections) {
        auto &&id = entry.first;
        auto &&connection = entry.second;
        auto &&address = connection->socket->get_address();
        view.emplace(id, address);
    }
    return view;
}

void TransferServer::kill(id_t id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&it = connections.find(id);
    unsafe_kill(it);
}

std::shared_ptr<Connection> TransferServer::connection_by_descriptor(socket_t descriptor) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&desc_entry = descriptors.find(descriptor);
    if (desc_entry == std::end(descriptors))
        throw TransferServer::error("connection hasn't found");
    auto &&id = desc_entry->second;
    auto &&conn_entry = connections.find(id);
    if (conn_entry == std::end(connections))
        throw TransferServer::error("connection hasn't found");
    auto &&connection = conn_entry->second;
    return connection;
}

connections_iterator TransferServer::unsafe_kill(connections_iterator it) {
    if (it == std::end(connections))
        return it;
    auto &&id = it->first;
    auto &&connection = it->second;
    auto &&socket = connection->socket;
    socket->shutdown();
    socket->close();
    it = connections.erase(it);
    disconnect_handle(id);
    return it;
}

std::string TransferServer::format(id_t id, std::shared_ptr<Socket> socket) {
    std::stringstream stream;
    stream << "[" << id << " " << socket << "]";
    return stream.str();
}

void TransferServer::broadcast(const char *message) {
    std::string m(message);
    broadcast(m);
}

void TransferServer::broadcast(const std::string &message) {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto &&entry: connections) {
        auto &&connection = entry.second;
        auto &&socket = connection->socket;
        socket->send(message);
    }
}

void TransferServer::send(const std::vector<id_t> &ids, const char *message) {
    std::string m(message);
    send(ids, m);
}

void TransferServer::send(const std::vector<id_t> &ids, const std::string &message) {
    for (auto &&id: ids) {
        send(id, message);
    }
}

void TransferServer::send(id_t id, const char *message) {
    std::string m(message);
    send(id, m);
}

void TransferServer::send(id_t id, const std::string &message) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&entry = connections.find(id);
    if (entry == std::end(connections))
        throw TransferServer::error("connection don't found");
    auto &&connection = entry->second;
    auto &&socket = connection->socket;
    socket->send(message);
}

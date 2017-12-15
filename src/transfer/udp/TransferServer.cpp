#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <set>
#include "../udp/Socket.h"
#include "TransferServer.h"
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
        const std::function<bool(identifier_t, const std::string &)> &handle
) {
    bool close = false;
    auto &&id = connection->id;
    auto &&socket = connection->socket;
    try {
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

void TransferServer::handle_connection(std::shared_ptr<Connection> connection, ThreadPool &pool) {
    std::unique_lock<std::mutex> socket_lock(connection->mutex);
    auto &&socket = connection->socket;
    if (!connection->free) return;
    connection->free = false;
    auto &&handle = [this](identifier_t id, const std::string &message) -> bool {
        this->handle(id, message);
    };
    pool.enqueue(task, connection, handle);
}

void TransferServer::run() {
    ThreadPool pool(NUM_THREADS);
    identifier_t max_id = 0;
    int epoll_descriptor = epoll_create(1);
    if (epoll_descriptor == -1)
        throw TransferServer::error("creation of epoll descriptor is impossible");
    add_descriptor(epoll_descriptor, socket->get_descriptor());
    epoll_event events[10];
    while (!stop_requests) {
        auto &&event_cnt = epoll_wait(epoll_descriptor, events, sizeof(events), TIMEOUT_MSEC);
        for (auto &&i = 0; i < event_cnt; ++i) {
            auto &&event = events[i];
            if (event.events & EPOLLERR) {
                std::cerr << TransferServer::format(0, socket) << " epoll error" << std::endl;
            }
            if (event.events & EPOLLIN) {
                auto &&result = socket->update();
                auto message = std::get<0>(result);
                auto address = std::get<1>(result);
                identifier_t id = Socket::concat(address);
                auto &&entry = connections.find(id);
                if (entry == std::end(connections)) {
                    auto &&client = socket->accept(address);
                    connections.emplace(id, std::make_shared<Connection>(id, client));
                    connect_handle(id);
                }
                auto &&connection = connections[id];
                connection->socket->update(message);
                if (!connection->socket->empty())
                    handle_connection(connection, pool);
            }
        }
        for (auto &&entry: connections) {
            auto &&connection = entry.second;
            if (connection->socket->update(TIMEOUT_MSEC))
                connection->close = true;
        }
        disconnect_unavailable_connections();
    }
    disconnect_connections();
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

std::unordered_map<identifier_t, std::shared_ptr<address_t>> TransferServer::get_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    std::unordered_map<identifier_t, std::shared_ptr<address_t>> view;
    for (auto &&entry: connections) {
        auto &&id = entry.first;
        auto &&connection = entry.second;
        auto &&address = connection->socket->get_address();
        view.emplace(id, address);
    }
    return view;
}

void TransferServer::kill(identifier_t id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&it = connections.find(id);
    unsafe_kill(it);
}

connections_iterator TransferServer::unsafe_kill(connections_iterator it) {
    try {
        if (it == std::end(connections))
            return it;
        auto &&id = it->first;
        auto &&connection = it->second;
        connection->socket->close();
        it = connections.erase(it);
        disconnect_handle(id);
    } catch (Socket::error &ignore) {}
    return it;
}

std::string TransferServer::format(identifier_t id, std::shared_ptr<Socket> socket) {
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

void TransferServer::send(const std::vector<identifier_t> &ids, const char *message) {
    std::string m(message);
    send(ids, m);
}

void TransferServer::send(const std::vector<identifier_t> &ids, const std::string &message) {
    for (auto &&id: ids) {
        send(id, message);
    }
}

void TransferServer::send(identifier_t id, const char *message) {
    std::string m(message);
    send(id, m);
}

void TransferServer::send(identifier_t id, const std::string &message) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&entry = connections.find(id);
    if (entry == std::end(connections))
        throw TransferServer::error("connection don't found");
    auto &&connection = entry->second;
    auto &&socket = connection->socket;
    socket->send(message);
}

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <set>
#include "Socket.h"
#include "TCPServer.h"
#include <sys/epoll.h>


TCPServer::TCPServer(int domain, int type, int protocol, address_t &address
) : socket(domain, type, protocol), stop_requests(true) {
    socket.set_options(SO_REUSEADDR);
    socket.bind(address);
    socket.listen(1);
}

TCPServer::~TCPServer() {
    stop();
    join();
}

bool TCPServer::start() {
    if (!stop_requests) return false;
    stop_requests = false;
    thread = std::thread([this] { this->run(); });
    return true;
}

void task(std::shared_ptr<Connection> connection, const handle_signature &handle) {
    bool close;
    auto &&id = connection->id;
    auto &&address = connection->address;
    Socket socket(connection->descriptor);
    try {
        auto &&message = socket.receive();
        close = handle(id, address, message);
    } catch (std::runtime_error &ex) {
        std::cerr << TCPServer::format(id, address) << " handle error: " << ex.what() << std::endl;
        close = true;
    } catch (...) {
        std::cerr << TCPServer::format(id, address) << " handle error" << std::endl;
        close = true;
    }
    std::unique_lock<std::mutex> socket_lock(connection->mutex);
    connection->close = close;
    connection->free = true;
    if (connection->close)
        socket.safe_shutdown();
}

void TCPServer::disconnect_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    connections_iterator it = std::begin(connections);
    while (it != std::end(connections))
        it = unsafe_kill(it);
}

void TCPServer::disconnect_unavailable_connections() {
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
        throw TCPServer::error("descriptor addition failed");
}

void TCPServer::handle_new_connection(int epoll_descriptor, id_t &max_id) {
    auto &&descriptor = socket.accept();
    std::unique_lock<std::mutex> lock(mutex);
    auto &&id = max_id++;
    Socket socket(descriptor);
    auto &&address = socket.get_address();
    auto &&connection_ptr = std::make_shared<Connection>(id, descriptor, address);
    connections.emplace(id, connection_ptr);
    descriptors.emplace(descriptor, id);
    add_descriptor(epoll_descriptor, descriptor);
    connect_handle(id, address);
}

void TCPServer::handle_connection(socket_t descriptor, ThreadPool &pool) {
    auto &&connection = connection_by_descriptor(descriptor);
    std::unique_lock<std::mutex> socket_lock(connection->mutex);
    if (!connection->free) return;
    connection->free = false;
    auto &&handle = [this](id_t id, address_t address, const std::string &message) -> bool {
        this->handle(id, address, message);
    };
    pool.enqueue(task, connection, handle);
}

void TCPServer::run() {
    ThreadPool pool(NUM_THREADS);
    id_t max_id = 0;
    int epoll_descriptor = epoll_create(1);
    if (epoll_descriptor == -1)
        throw TCPServer::error("creation of epoll descriptor is impossible");
    add_descriptor(epoll_descriptor, socket.descriptor);
    epoll_event events[10];
    while (!stop_requests) {
        auto &&event_cnt = epoll_wait(epoll_descriptor, events, sizeof(events), TIMEOUT_USEC);
        for (auto &&i = 0; i < event_cnt; ++i) {
            auto &&event = events[i];
            auto &&descriptor = (socket_t) event.data.fd;
            if (event.events & EPOLLERR) {
                remove_descriptor(epoll_descriptor, event);
                throw TCPServer::error("epoll error");
            }
            if (event.events & EPOLLHUP) {
                remove_descriptor(epoll_descriptor, event);
                auto &&connection = connection_by_descriptor(descriptor);
                connection->close = true;
            }
            if (event.events & EPOLLIN) {
                if (descriptor == socket.descriptor) {
                    handle_new_connection(epoll_descriptor, max_id);
                } else {
                    handle_connection(descriptor, pool);
                }
            }
        }
        disconnect_unavailable_connections();
    }
    disconnect_connections();
    socket.close();
}

void TCPServer::join() {
    if (thread.joinable()) {
        thread.join();
    }
}

void TCPServer::stop() {
    stop_requests = true;
}

bool TCPServer::stopped() {
    return stop_requests;
}

std::unordered_map<id_t, address_t> TCPServer::get_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    std::unordered_map<id_t, address_t> view;
    for (auto &&entry: connections) {
        auto &&id = entry.first;
        auto &&connection = entry.second;
        view.emplace(id, connection->address);
    }
    return view;
}

void TCPServer::kill(id_t id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&it = connections.find(id);
    unsafe_kill(it);
}

std::shared_ptr<Connection> TCPServer::connection_by_descriptor(socket_t descriptor) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&desc_entry = descriptors.find(descriptor);
    if (desc_entry == std::end(descriptors))
        throw TCPServer::error("connection don't found");
    auto &&id = desc_entry->second;
    auto &&conn_entry = connections.find(id);
    if (conn_entry == std::end(connections))
        throw TCPServer::error("connection don't found");
    auto &&connection = conn_entry->second;
    return connection;
}

connections_iterator TCPServer::unsafe_kill(connections_iterator it) {
    if (it == std::end(connections)) return it;
    auto &&id = it->first;
    auto &&connection = it->second;
    Socket socket(connection->descriptor);
    socket.safe_shutdown();
    socket.safe_close();
    it = connections.erase(it);
    descriptors.erase(connection->descriptor);
    disconnect_handle(id, connection->address);
    return it;
}

std::string TCPServer::format(id_t id, address_t address) {
    std::stringstream stream;
    stream << "[" << id << " " << address << "]";
    return stream.str();
}

void TCPServer::broadcast(const char *message) {
    std::string m(message);
    broadcast(m);
}

void TCPServer::broadcast(const std::string &message) {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto &&entry: connections) {
        auto &&connection = entry.second;
        auto &&descriptor = connection->descriptor;
        Socket socket(descriptor);
        socket.send(message);
    }
}

void TCPServer::send(const std::vector<id_t> &ids, const char *message) {
    std::string m(message);
    send(ids, m);
}

void TCPServer::send(const std::vector<id_t> &ids, const std::string &message) {
    for (auto &&id: ids) {
        send(id, message);
    }
}

void TCPServer::send(id_t id, const char *message) {
    std::string m(message);
    send(id, m);
}

void TCPServer::send(id_t id, const std::string &message) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&entry = connections.find(id);
    if (entry == std::end(connections))
        throw TCPServer::error("connection don't found");
    auto &&connection = entry->second;
    auto &&descriptor = connection->descriptor;
    Socket socket(descriptor);
    socket.send(message);
}

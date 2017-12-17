#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <set>
#include "../udp/Socket.h"
#include "TransferServer.h"


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
    auto &&connections = get_connections();
    for (auto &&entry: connections)
        kill(entry.first);
}

void TransferServer::disconnect_unavailable_connections() {
    auto &&connections = get_connections();
    for (auto &&entry: connections) {
        auto &&id = entry.first;
        auto &&connection = get_connection(id);
        if (connection->close)
            kill(id);
    }
}

void TransferServer::handle_connection(std::shared_ptr<Connection> connection, ThreadPool &pool) {
    std::unique_lock<std::mutex> socket_lock(connection->mutex);
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
    while (!stop_requests) {
        timeval timeout{0, TIMEOUT_MSEC * 1000};
        if (socket->select(&timeout)) {
            try {
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
                auto &&connection = connections.at(id);
                if (connection->socket->update(message))
                    connection->close = true;
                else if (!connection->socket->empty())
                    handle_connection(connection, pool);
            } catch (Socket::error &ex) {
                std::cerr << "update error: " << ex.what() << std::endl;
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

std::unordered_map<identifier_t, address_t> TransferServer::get_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    std::unordered_map<identifier_t, address_t> view;
    for (auto &&entry: connections) {
        auto &&id = entry.first;
        auto &&connection = entry.second;
        auto &&address = connection->socket->get_address();
        view.emplace(id, address);
    }
    return view;
}

void TransferServer::kill(identifier_t id) {
    disconnect_handle(id);
    std::unique_lock<std::mutex> lock(mutex);
    auto &&it = connections.find(id);
    auto &&connection = it->second;
    connection->socket->close();
    it = connections.erase(it);
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
    auto &&connection = get_connection(id);
    auto &&socket = connection->socket;
    socket->send(message);
}

std::shared_ptr<Connection> TransferServer::get_connection(identifier_t id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&entry = connections.find(id);
    if (entry == std::end(connections))
        throw TransferServer::error("connection don't found");
    return entry->second;
}

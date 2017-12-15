#pragma once

#include "../tcp/Socket.h"
#include "thread_pool/ThreadPool.h"
#include <thread>
#include <unordered_map>
#include <mutex>
#include <utility>

using id_t = uint64_t;

class Connection {
public:
    const id_t id;

    const std::shared_ptr<Socket> socket;

    bool free;

    bool close;

    std::mutex mutex;

    Connection(id_t id, std::shared_ptr<Socket> socket) : // NOLINT
            id(id), socket(socket), free(true), close(false) {}
};

using connections_iterator = std::unordered_map<id_t, std::shared_ptr<Connection>>::const_iterator;

class TransferServer {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

public:
    static const size_t NUM_THREADS = 4;

    static const size_t TIMEOUT_USEC = 1000;

private:
    std::shared_ptr<Socket> socket;

    std::thread thread;

    bool stop_requests;

    std::unordered_map<socket_t, id_t> descriptors;

    std::unordered_map<id_t, std::shared_ptr<Connection>> connections;

    std::mutex mutex;

public:
    explicit TransferServer(std::shared_ptr<Socket> socket);

    virtual ~TransferServer();

    bool start();

    virtual void join();

    void stop();

    bool stopped();

    std::unordered_map<id_t, std::shared_ptr<address_t>> get_connections();

    void kill(id_t id);

    virtual void broadcast(const char *message);

    virtual void broadcast(const std::string &message);

    virtual void send(id_t id, const char *message);

    virtual void send(id_t id, const std::string &message);

    virtual void send(const std::vector<id_t> &ids, const char *message); // NOLINT

    virtual void send(const std::vector<id_t> &ids, const std::string &message); // NOLINT

protected:
    virtual bool handle(id_t id, const std::string &message) = 0;

    virtual void connect_handle(id_t id) = 0;

    virtual void disconnect_handle(id_t id) = 0;

private:
    void run();

    void disconnect_connections();

    void disconnect_unavailable_connections();

    void handle_new_connection(int epoll_descriptor, id_t &max_id);

    void handle_connection(std::shared_ptr<Connection> connection, ThreadPool &pool);

    std::shared_ptr<Connection> connection_by_descriptor(socket_t descriptor);

    connections_iterator unsafe_kill(connections_iterator it);

public:
    static std::string format(id_t id, std::shared_ptr<Socket> socket);
};

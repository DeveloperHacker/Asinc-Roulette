#pragma once

#include "Socket.h"
#include "../lib/thread_pool/ThreadPool.h"
#include <thread>
#include <unordered_map>
#include <mutex>

struct Connection {
    id_t id;

    socket_t descriptor;

    address_t address;

    bool free;

    bool close;

    std::shared_ptr<std::mutex> mutex;
};

using connections_iterator = std::unordered_map<id_t, Connection>::const_iterator;

using handle_signature = std::function<bool(id_t, address_t, const std::string &)>;

class TCPServer {
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
    Socket socket;

    std::thread thread;

    bool stop_requests;

    std::unordered_map<socket_t, id_t> descriptors;

    std::unordered_map<id_t, Connection> connections;

    std::mutex mutex;

public:
    TCPServer(int domain, int type, int protocol, address_t &address);

    virtual ~TCPServer();

    bool start();

    void join();

    void stop();

    bool stopped();

    std::unordered_map<id_t, address_t> get_connections();

    void kill(id_t id);

    virtual void broadcast(const char *message);

    virtual void broadcast(const std::string &message);

    virtual void send(id_t id, const char *message);

    virtual void send(id_t id, const std::string &message);

    virtual void send(const std::vector<id_t> &ids, const char *message);

    virtual void send(const std::vector<id_t> &ids, const std::string &message);

protected:
    virtual bool handle(id_t id, address_t address, const std::string &message) = 0;

    virtual void connect_handle(id_t id, address_t address) = 0;

    virtual void disconnect_handle(id_t id, address_t address) = 0;

private:
    void run();

    void disconnect_connections();

    void disconnect_unavailable_connections();

    void handle_new_connection(int epoll_descriptor, id_t &max_id);

    void handle_connection(socket_t descriptor, ThreadPool &pool);

    Connection &connection_by_descriptor(socket_t descriptor);

    connections_iterator unsafe_kill(connections_iterator it);

public:
    static std::string format(id_t id, address_t address);
};

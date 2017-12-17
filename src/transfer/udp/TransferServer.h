#pragma once

#include "../udp/Socket.h"
#include "thread_pool/ThreadPool.h"
#include <thread>
#include <unordered_map>
#include <mutex>
#include <utility>

using identifier_t = uint64_t;

class Connection {
public:
    const identifier_t id;

    const std::shared_ptr<Socket> socket;

    bool free;

    bool close;

    std::mutex mutex;

    Connection(identifier_t id, std::shared_ptr<Socket> socket) : // NOLINT
            id(id), socket(socket), free(true), close(false) {}
};

using connections_iterator = std::unordered_map<identifier_t, std::shared_ptr<Connection>>::const_iterator;

class TransferServer {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

public:
    static const size_t NUM_THREADS = 4;

    static const size_t TIMEOUT_MSEC = 1000;

private:
    std::shared_ptr<Socket> socket;

    std::thread thread;

    bool stop_requests;

    std::unordered_map<identifier_t, std::shared_ptr<Connection>> connections;

    std::mutex mutex;

public:
    explicit TransferServer(std::shared_ptr<Socket> socket);

    virtual ~TransferServer();

    bool start();

    virtual void join();

    void stop();

    bool stopped();

    std::unordered_map<identifier_t, address_t> get_connections();

    void kill(identifier_t id);

    virtual void broadcast(const char *message);

    virtual void broadcast(const std::string &message);

    virtual void send(identifier_t id, const char *message);

    virtual void send(identifier_t id, const std::string &message);

    virtual void send(const std::vector<identifier_t> &ids, const char *message); // NOLINT

    virtual void send(const std::vector<identifier_t> &ids, const std::string &message); // NOLINT

protected:
    virtual bool handle(identifier_t id, const std::string &message) = 0;

    virtual void connect_handle(identifier_t id) = 0;

    virtual void disconnect_handle(identifier_t id) = 0;

private:
    void run();

    void disconnect_connections();

    void disconnect_unavailable_connections();
    
    void handle_connection(std::shared_ptr<Connection> connection, ThreadPool &pool);
    
    std::shared_ptr<Connection> get_connection(identifier_t id);

public:
    static std::string format(identifier_t id, std::shared_ptr<Socket> socket);
};

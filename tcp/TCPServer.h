#pragma once


#include "SendSocket.h"
#include "../lib/thread_pool/ThreadPool.h"
#include <thread>
#include <unordered_map>
#include <mutex>


class TCPServer {
public:
    struct State {
        socket_t descriptor;

        address_t address;

        bool free;

        bool close;

        std::mutex *mutex;
    };

public:
    static const size_t NUM_THREADS = 4;

    static const size_t TIMEOUT_SEC = 0;

    static const size_t TIMEOUT_USEC = 1000;

private:
    Socket socket;

    std::thread thread;

    bool stop_requests;

    std::unordered_map<socket_t, id_t> descriptors;

    std::unordered_map<id_t, State> connections;

    std::mutex mutex;

public:
    TCPServer(int domain, int type, int protocol, address_t &address);

    virtual ~TCPServer();

    bool start();

    void join();

    void stop();

    bool stopped();

    std::unordered_map<id_t, address_t> get_connections();

    int kill(id_t id);

protected:
    virtual bool handle(const std::string &message, id_t id, SendSocket &socket) = 0;

    virtual void connect_handle(id_t id) = 0;

    virtual void disconnect_handle(id_t id) = 0;

private:
    void run();

    fd_set descriptor_set();

    void disconnect_connections();

    void disconnect_unavailable_connections();

    address_t handle_new_connection(int epoll_descriptor, id_t &max_id);

    void handle_connection(socket_t descriptor, ThreadPool &pool);

    State &state_by_descriptor(socket_t descriptor);

    id_t id_by_descriptor(socket_t descriptor);

    State &state_by_id(id_t id);
};

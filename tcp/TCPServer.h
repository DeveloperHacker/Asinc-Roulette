#pragma once


#include "SendSocket.h"
#include <thread>
#include <unordered_map>
#include <mutex>


class TCPServer {
public:
    struct State {
        socket_t descriptor;

        std::mutex &mutex;

        address_t address;

        bool free;

        bool close;
    };

public:
    static const size_t NUM_THREADS = 4;

    static const size_t TIMEOUT_SEC = 0;

    static const size_t TIMEOUT_USEC = 1000;

private:
    Socket socket;

    std::thread thread;

    bool stop_requests;

    std::unordered_map<socket_t, State> descriptors;

    std::mutex descriptors_mutex;

public:
    TCPServer(int domain, int type, int protocol, address_t &address);

    virtual ~TCPServer();

    bool start();

    void join();

    void stop();

    bool stopped();

    std::unordered_map<int, address_t> get_descriptors();

    int kill(socket_t descriptor);

protected:
    virtual bool handle(const std::string &message, SendSocket &socket) = 0;

private:
    void run();

    fd_set descriptor_set();

    void cleanup();
};

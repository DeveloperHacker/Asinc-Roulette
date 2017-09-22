#pragma once


#include "SafeSocket.h"
#include <thread>
#include <unordered_map>
#include <mutex>


struct State {
    address_t address{};
    bool free = true;
    bool close = false;
};

class TCPServer {
public:
    static const size_t NUM_THREADS = 4;
    static const size_t TIMEOUT_SEC = 1;
    static const size_t TIMEOUT_USEC = 0;
private:
    Socket socket;
    const address_t address;
    std::thread thread;
    bool stop_requests;
    std::unordered_map<int, State> descriptors;
    std::mutex descriptors_mutex;

public:
    TCPServer(int domain, int type, int protocol, address_t &address);

    virtual ~TCPServer();

    void run();

    void join();

    void stop();

    virtual bool handle(SafeSocket &socket) = 0;

    bool is_stopped();

    std::unordered_map<int, address_t> get_descriptors();

    int kill(socket_t descriptor);

private:
    fd_set descriptor_set();
};

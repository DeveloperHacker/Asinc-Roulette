#pragma once


#include "SafeSocket.h"
#include <thread>
#include <unordered_map>
#include <mutex>


struct State {
    std::mutex &mutex;

    address_t address;

    bool free;

    bool close;
};

class TCPServer {
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

    void run();

    void join();

    void stop();

    bool stopped();

    std::unordered_map<int, address_t> get_descriptors();

    int kill(socket_t descriptor);

protected:
    virtual bool handle(SafeSocket &socket) = 0;

private:
    fd_set descriptor_set();
    
    void cleanup();
};

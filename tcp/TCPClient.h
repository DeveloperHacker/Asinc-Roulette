#pragma once

#include <thread>
#include <iostream>
#include <mutex>
#include "../../tcp/Socket.h"

class TCPClient {
private:
    bool stop_requests;
    Socket socket;
    std::thread thread;
    std::mutex mutex;

public:
    TCPClient(int domain, int type, int protocol, address_t &address) :
            socket(domain, type, protocol),
            stop_requests(false),
            thread([this] { this->run(); }) {
        socket.connect(address);
    }

    void run() {
        while (!stop_requests) {
            try {
                handle(socket);
            } catch (std::exception &ex) {
                std::cerr << ex.what() << std::endl;
            }
            std::unique_lock lock(mutex);
        }
    }

    void shutdown() {
        std::unique_lock lock(mutex);
        stop_requests = true;
        socket.shutdown();
        socket.raw_close();
    }

    virtual bool handle(Socket socket);
};

#pragma once

#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include "Socket.h"
#include "SendSocket.h"

using status_t = int;

class TCPClient {
public:
    static const status_t WAIT = 0;
    static const status_t SEND = 1;

private:
    bool stop_requests;

    Socket socket;

    std::thread input_thread;

    std::thread output_thread;

    std::mutex mutex;

public:
    TCPClient(int domain, int type, int protocol, const address_t &address);

    bool start();

    void join();

    void stop();

protected:
    virtual void input(const std::string &message) = 0;

    virtual void output(SendSocket &socket) = 0;

private:
    void safe_run(const std::function<void()> &function);
};

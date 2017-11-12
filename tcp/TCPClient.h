#pragma once

#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include "Socket.h"

class TCPClient {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };
    
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

    virtual void send(const char *message);

    virtual void send(const std::string &message);

protected:
    virtual void input(const std::string &message) = 0;

    virtual void output() = 0;

private:
    void safe_run(const std::function<void()> &function);
};

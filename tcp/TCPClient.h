#pragma once

#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include "Socket.h"

class Task {
public:
    const std::string message;

public:
    explicit Task(const char *message) : message(message) {}

    explicit Task(const std::string &message) : message(message) {}

    ~Task() = default;
};

class TCPClient {
private:
    bool stop_requests;

    Socket socket;

    std::thread input_thread;

    std::thread output_thread;

    std::mutex mutex;

public:
    TCPClient(int domain, int type, int protocol, const address_t &address);

    void join();

    void stop();

protected:
    virtual void input(const Task &task) = 0;

    virtual Task output() = 0;

private:
    void safe_run(const std::function<void()> &function);
};

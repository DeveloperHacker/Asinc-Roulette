#pragma once

#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include "Socket.h"

class TransferClient {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

private:
    bool stop_requests;

    std::shared_ptr<Socket> socket;

    std::thread input_thread;

    std::thread output_thread;

    std::mutex mutex;

public:
    explicit TransferClient(std::shared_ptr<Socket> socket);

    bool start();

    virtual void join();

    void stop();

    virtual void send(const char *message);

    virtual void send(const std::string &message);

protected:
    virtual void input(const std::string &message) = 0;

    virtual void output() = 0;

private:
    void loop(const std::function<void()> &function);
};

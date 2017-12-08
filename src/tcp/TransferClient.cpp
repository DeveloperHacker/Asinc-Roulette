#include <functional>
#include "TransferClient.h"
#include "TransferServer.h"

void TransferClient::loop(const std::function<void()> &function) {
    while (!stop_requests) {
        try {
            function();
        } catch (Socket::error &ex) {
            std::cerr << ex.what() << std::endl;
            stop();
        }
    }
    stop();
}

TransferClient::TransferClient(std::shared_ptr<Socket> socket) : socket(socket), stop_requests(true) {}

bool TransferClient::start() {
    if (!stop_requests) return false;
    stop_requests = false;
    input_thread = std::thread([this] {
        this->loop([this] {
            timeval timeout{0, TransferServer::TIMEOUT_USEC};
            if (socket->select(&timeout)) {
                socket->update(Socket::Event::DATA_IN);
                while (!socket->empty()) {
                    auto &&message = socket->receive();
                    input(message);
                }
            } else {
                socket->update(Socket::Event::TIMEOUT);
            }
        });
        if (output_thread.joinable())
            output_thread.detach();
    });
    output_thread = std::thread([this] {
        this->loop([this]() {
            output();
        });
    });
    return true;
}

void TransferClient::join() {
    if (input_thread.joinable())
        input_thread.join();
    if (output_thread.joinable())
        output_thread.join();
}

void TransferClient::stop() {
    std::unique_lock<std::mutex> lock(mutex);
    if (!stop_requests) {
        stop_requests = true;
        socket->shutdown();
        socket->close();
    }
}

void TransferClient::send(const char *message) {
    socket->send(message);
}

void TransferClient::send(const std::string &message) {
    socket->send(message);
}

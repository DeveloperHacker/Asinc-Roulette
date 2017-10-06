#include <functional>
#include "TCPClient.h"

void TCPClient::safe_run(const std::function<void()> &function) {
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

TCPClient::TCPClient(int domain, int type, int protocol, const address_t &address) :
        socket(domain, type, protocol),
        stop_requests(false) {
    socket.connect(address);
    input_thread = std::thread([this] {
        this->safe_run([this] {
            auto &&ready = socket.select();
            if (!ready) return;
            auto &&message = socket.receive();
            Task task(message);
            input(task);
        });
    });
    output_thread = std::thread([this] {
        this->safe_run([this]() {
            auto &&task = output();
            auto &&message = task.message;
            socket.send(message);
        });
    });
}

void TCPClient::join() {
    if (input_thread.joinable()) {
        input_thread.join();
    }
    if (output_thread.joinable()) {
        output_thread.join();
    }
}

void TCPClient::stop() {
    std::unique_lock<std::mutex> lock(mutex);
    if (!stop_requests) {
        stop_requests = true;
        socket.raw_shutdown();
        socket.raw_close();
    }
}

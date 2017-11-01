#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <set>
#include "Socket.h"
#include "ThreadPool.h"
#include "TCPServer.h"


TCPServer::TCPServer(int domain, int type, int protocol, address_t &address
) : socket(domain, type, protocol), stop_requests(true) {
    socket.set_options(SO_REUSEADDR);
    socket.bind(address);
    socket.listen(1);
}

TCPServer::~TCPServer() {
    stop();
    join();
}

bool TCPServer::start() {
    if (!stop_requests) return false;
    stop_requests = false;
    thread = std::thread([this] { this->run(); });
    return true;
}

void task(TCPServer::State &state, std::function<bool(const std::string &, SendSocket &)> handle) {
    bool close;
    Socket socket(state.descriptor);
    try {
        auto &&message = socket.receive();
        std::cout << "Message receive " << socket << " " << message.c_str() << std::endl;
        SendSocket send_socket(state.descriptor);
        close = handle(message, send_socket);
    } catch (Socket::error &ex) {
        std::cerr << "Socket '" << socket << "' error '" << ex.what() << "'" << std::endl;
        close = true;
    }
    std::unique_lock<std::mutex> lock(state.mutex);
    state.close = close;
    state.free = true;
    if (state.close) {
        socket.raw_close();
    }
}

void TCPServer::cleanup() {
    std::unique_lock<std::mutex> descriptors_lock(descriptors_mutex);
    auto &&it = std::begin(descriptors);
    while (it != std::end(descriptors)) {
        auto &&entry = *it;
        auto &&descriptor = entry.first;
        auto &&state = entry.second;
        std::unique_lock<std::mutex> lock(state.mutex);
        if (state.free && state.close) {
            it = descriptors.erase(it);
            continue;
        }
        ++it;
    }
}

void TCPServer::run() {
    ThreadPool pool(NUM_THREADS);
    socket_t max_descriptor = socket.descriptor;
    while (!stop_requests) {
        try {
            fd_set read_fd_set = this->descriptor_set();
            timeval timeout{TIMEOUT_SEC, TIMEOUT_USEC};
            int ready = ::select(max_descriptor + 1, &read_fd_set, nullptr, nullptr, &timeout);
            if (ready < 0) throw Socket::error("select is ripped");
            cleanup();
            if (ready == 0) continue;
            if (FD_ISSET(socket.descriptor, &read_fd_set)) {
                auto &&descriptor = socket.accept();
                if (descriptor > max_descriptor) max_descriptor = descriptor;
                std::mutex mutex;
                auto &&address = Socket(descriptor).get_address();
                Socket socket(descriptor);
                State state{descriptor, mutex, address, true, false};
                std::unique_lock<std::mutex> lock(descriptors_mutex);
                descriptors.emplace(descriptor, state);
                std::cout << "Connected client: " << socket << std::endl;
            } else {
                std::unique_lock<std::mutex> descriptors_lock(descriptors_mutex);
                for (auto &&entry : descriptors) {
                    auto &&descriptor = entry.first;
                    auto &&state = entry.second;
                    std::unique_lock<std::mutex> lock(state.mutex);
                    if (!FD_ISSET(descriptor, &read_fd_set)) continue;
                    if (!state.free) continue;
                    state.free = false;
                    auto &&handle = [this](const std::string &message, SendSocket &socket) -> bool {
                        this->handle(message, socket);
                    };
                    pool.enqueue(task, std::ref(state), handle);
                }
            }
        } catch (Socket::error &ex) {
            std::cerr << "Server error " << ex.what() << std::endl;
        }
    }
    std::condition_variable event;
    for (auto &&entry : descriptors) {
        auto &&descriptor = entry.first;
        auto &&state = entry.second;
        Socket socket(descriptor);
        socket.raw_shutdown();
        socket.raw_close();
        std::unique_lock<std::mutex> lock(state.mutex);
        event.wait(lock, [&state] { return state.free; });
    }
    socket.close();
}

void TCPServer::join() {
    if (thread.joinable()) {
        thread.join();
    }
}

void TCPServer::stop() {
    stop_requests = true;
}

bool TCPServer::stopped() {
    return stop_requests;
}

fd_set TCPServer::descriptor_set() {
    std::unique_lock<std::mutex> lock(descriptors_mutex);
    fd_set view{};
    FD_ZERO(&view);
    FD_SET(socket.descriptor, &view);
    for (auto &&entry: descriptors) {
        auto &&descriptor = entry.first;
        auto &&state = entry.second;
        FD_SET(descriptor, &view);
    }
    return view;
};

std::unordered_map<socket_t, address_t> TCPServer::get_descriptors() {
    std::unique_lock<std::mutex> lock(descriptors_mutex);
    std::unordered_map<socket_t, address_t> view;
    for (auto &&entry: descriptors) {
        auto &&descriptor = entry.first;
        auto &&state = entry.second;
        view.emplace(descriptor, state.address);
    }
    return view;
}

int TCPServer::kill(socket_t descriptor) {
    std::unique_lock<std::mutex> lock(descriptors_mutex);
    if (descriptors.count(descriptor) == 0) return 1;
    descriptors.erase(descriptor);
    Socket socket(descriptor);
    socket.raw_shutdown();
    return socket.raw_close();
}



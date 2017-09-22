#include <iostream>
#include <sstream>
#include <unordered_map>
#include <set>
#include "Socket.h"
#include "ThreadPool.h"
#include "TCPServer.h"


TCPServer::TCPServer(int domain, int type, int protocol, address_t &address) :
        socket(domain, type, protocol),
        address(address),
        stop_requests(false),
        thread([this] { this->run(); }) {
}

TCPServer::~TCPServer() {
    stop();
    join();
}


void task(socket_t descriptor, std::mutex &mutex, State &state, std::function<bool(SafeSocket &)> handle) {
    bool close;
    try {
        SafeSocket socket(descriptor);
        close = handle(socket);
    } catch (Socket::error &ex) {
        std::stringstream format;
        format << "[TCPServer::task] " << ex.what() << std::endl;
        std::cerr << format.str();
        close = true;
    }
    {
        std::unique_lock<std::mutex> lock(mutex);
        state.close = close;
        state.free = true;
    }
}

void TCPServer::run() {

    ThreadPool pool(NUM_THREADS);

    socket.bind(address);
    socket.listen(1);

    socket_t max_descriptor = socket.descriptor;

    std::mutex mutex;
    while (!stop_requests) {
        try {
            fd_set read_fd_set = this->descriptor_set();
            timeval timeout{TIMEOUT_SEC, TIMEOUT_USEC};
            int ready = select(max_descriptor + 1, &read_fd_set, nullptr, nullptr, &timeout);
            if (ready < 0) throw Socket::error("select is ripped");
            if (ready == 0) continue;
            if (FD_ISSET(socket.descriptor, &read_fd_set)) {
                auto &&descriptor = socket.accept();
                if (descriptor > max_descriptor) max_descriptor = descriptor;
                State state{};
                state.address = Socket(descriptor).get_address();
                std::unique_lock<std::mutex> lock(descriptors_mutex);
                descriptors.emplace(descriptor, state);
            } else {
                std::unique_lock<std::mutex> descriptors_lock(descriptors_mutex);
                auto &&it = std::begin(descriptors);
                while (it != std::end(descriptors)) {
                    auto &&entry = *it;
                    auto &&descriptor = entry.first;
                    auto &&state = entry.second;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        if (state.close) {
                            Socket(descriptor).raw_close();
                            it = descriptors.erase(it);
                            continue;
                        } else {
                            ++it;
                        }
                        if (!FD_ISSET(descriptor, &read_fd_set)) continue;
                        if (!state.free) continue;
                        state.free = false;
                    }
                    pool.enqueue(task, descriptor, std::ref(mutex), std::ref(state),
                                 [this](SafeSocket &socket) -> bool {
                                     this->handle(socket);
                                 });
                }
            }
        } catch (Socket::error &ex) {
            std::stringstream format;
            format << "[TCPServer::loop] " << ex.what() << std::endl;
            std::cerr << format.str();
        }
    }
    std::condition_variable event;
    for (auto &&entry : descriptors) {
        auto &&descriptor = entry.first;
        auto &&state = entry.second;
        Socket socket(descriptor);
        socket.raw_close();
        std::unique_lock<std::mutex> lock(mutex);
        event.wait(lock, [&state] { return state.free; });
    }
    socket.close();
}

void TCPServer::join() {
    if (thread.joinable())
        thread.join();
}

void TCPServer::stop() {
    stop_requests = true;
}

bool TCPServer::is_stopped() {
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
    return Socket(descriptor).raw_close();
}



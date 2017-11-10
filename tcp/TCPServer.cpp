#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <set>
#include "Socket.h"
#include "TCPServer.h"
#include <sys/epoll.h>


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
        std::cout << "message receive " << socket << " " << message.c_str() << std::endl;
        SendSocket send_socket(state.descriptor);
        close = handle(message, send_socket);
    } catch (Socket::error &ex) {
        std::cerr << "socket " << socket << " error " << ex.what() << std::endl;
        close = true;
    }
    std::unique_lock<std::mutex> socket_lock(*state.mutex);
    state.close = close;
    state.free = true;
    if (state.close) {
        socket.safe_close();
    }
}

void TCPServer::disconnect_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    std::condition_variable event;
    for (auto &&entry : connections) {
        auto &&id = entry.first;
        auto &&state = entry.second;
        Socket socket(state.descriptor);
        socket.safe_shutdown();
        socket.safe_close();
        std::unique_lock<std::mutex> socket_lock(*state.mutex);
        event.wait(socket_lock, [&state] { return state.free; });
        connections.erase(id);
        descriptors.erase(state.descriptor);
        delete state.mutex;
    }
}

void TCPServer::disconnect_unavailable_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&it = std::begin(connections);
    while (it != std::end(connections)) {
        auto &&entry = *it;
        auto &&id = entry.first;
        auto &&state = entry.second;
        std::unique_lock<std::mutex> socket_lock(*state.mutex);
        if (state.close) {
            it = connections.erase(it);
            descriptors.erase(state.descriptor);
            delete state.mutex;
            disconnect_handle(id);
            continue;
        }
        ++it;
    }
}

void remove_descriptor(int epoll_descriptor, epoll_event event) {
    epoll_ctl(epoll_descriptor, EPOLL_CTL_DEL, event.data.fd, &event);
}

void add_descriptor(int epoll_descriptor, socket_t descriptor) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLHUP;
    event.data.fd = descriptor;
    auto &&ctl_stat = epoll_ctl(epoll_descriptor, EPOLL_CTL_ADD, descriptor, &event);
    if (ctl_stat == -1) throw Socket::error("descriptor addition failed");
}

address_t TCPServer::handle_new_connection(int epoll_descriptor, id_t &max_id) {
    auto &&descriptor = socket.accept();
    auto &&id = max_id++;
    connect_handle(id);
    Socket socket(descriptor);
    auto &&address = socket.get_address();
    std::unique_lock<std::mutex> lock(mutex);
    State state{descriptor, address, true, false, new std::mutex};
    connections.emplace(id, state);
    descriptors.emplace(descriptor, id);
    add_descriptor(epoll_descriptor, descriptor);
    return address;
}

void TCPServer::handle_connection(socket_t descriptor, ThreadPool &pool) {
    auto &&id = id_by_descriptor(descriptor);
    auto &&state = state_by_id(id);
    std::unique_lock<std::mutex> lock(mutex);
    std::unique_lock<std::mutex> socket_lock(*state.mutex);
    if (!state.free) return;
    state.free = false;
    auto &&handle = [this, id](const std::string &message, SendSocket &socket) -> bool {
        this->handle(message, id, socket);
    };
    pool.enqueue(task, std::ref(state), handle);
}

void TCPServer::run() {
    std::cout << "server start" << std::endl;
    ThreadPool pool(NUM_THREADS);
    id_t max_id = 0;
    int epoll_descriptor = epoll_create(1);
    if (epoll_descriptor == -1) throw Socket::error("creation of epoll descriptor is impossible");
    add_descriptor(epoll_descriptor, socket.descriptor);
    epoll_event events[10];
    while (!stop_requests) {
        auto &&event_cnt = epoll_wait(epoll_descriptor, events, sizeof(events), TIMEOUT_USEC);
        for (auto &&i = 0; i < event_cnt; ++i) {
            auto &&event = events[i];
            auto &&descriptor = (socket_t) event.data.fd;
            if (event.events & EPOLLERR) {
                remove_descriptor(epoll_descriptor, event);
                auto &&state = state_by_descriptor(descriptor);
                std::unique_lock<std::mutex> state_lock(*(state.mutex));
                std::cout << "epoll error in connection " << state.address << std::endl;
                state.close = true;
            }
            if (event.events & EPOLLHUP) {
                remove_descriptor(epoll_descriptor, event);
                auto &&state = state_by_descriptor(descriptor);
                std::unique_lock<std::mutex> state_lock(*(state.mutex));
                std::cout << "client disconnected " << state.address << std::endl;
                state.close = true;
            }
            if (event.events & EPOLLIN) {
                if (descriptor == socket.descriptor) {
                    auto &&address = handle_new_connection(epoll_descriptor, max_id);
                    std::cout << "client connected " << address << std::endl;
                } else {
                    handle_connection(descriptor, pool);
                }
            }
        }
        disconnect_unavailable_connections();
    }
    disconnect_connections();
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
    std::unique_lock<std::mutex> lock(mutex);
    fd_set view{};
    FD_ZERO(&view);
    FD_SET(socket.descriptor, &view);
    for (auto &&entry: connections) {
        auto &&id = entry.first;
        auto &&state = entry.second;
        FD_SET(state.descriptor, &view);
    }
    return view;
};

std::unordered_map<id_t, address_t> TCPServer::get_connections() {
    std::unique_lock<std::mutex> lock(mutex);
    std::unordered_map<id_t, address_t> view;
    for (auto &&entry: connections) {
        auto &&id = entry.first;
        auto &&state = entry.second;
        view.emplace(id, state.address);
    }
    return view;
}

int TCPServer::kill(id_t id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&entry_ptr = connections.find(id);
    if (entry_ptr == std::end(connections)) return 1;
    auto &&state = entry_ptr->second;
    connections.erase(id);
    descriptors.erase(state.descriptor);
    delete state.mutex;
    Socket socket(state.descriptor);
    socket.safe_shutdown();
    return socket.safe_close();
}

TCPServer::State &TCPServer::state_by_descriptor(socket_t descriptor) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&desc_entry = descriptors.find(descriptor);
    auto &&id = desc_entry->second;
    auto &&conn_entry = connections.find(id);
    auto &&state = conn_entry->second;
    return state;
}

id_t TCPServer::id_by_descriptor(socket_t descriptor) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&entry = descriptors.find(descriptor);
    auto &&id = entry->second;
    return id;
}

TCPServer::State &TCPServer::state_by_id(id_t id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto &&entry = connections.find(id);
    auto &&state = entry->second;
    return state;
}

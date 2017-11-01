
#include <iostream>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Socket.h"

Socket::Socket(int domain, int type, int protocol) : descriptor(::socket(domain, type, protocol)) {}

Socket::Socket(socket_t socket) : descriptor(socket) {

}

Socket::~Socket() = default;

void Socket::bind(const address_t &address) {
    auto success = ::bind(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0) throw Socket::error("bind: address already used");
}

void Socket::listen(int backlog) {
    auto success = ::listen(descriptor, backlog);
    if (success < 0) throw Socket::error("listen: error");
}

void Socket::connect(const address_t &address) {
    auto success = ::connect(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0) throw Socket::error("connect: connection hast'n exist");
}

int raw_receive(socket_t socket, char *message, size_t length, int flags) {
    int received = 0;
    while (received < length) {
        char buffer[length];
        auto size = ::recv(socket, buffer, length, flags);
        if (size <= 0) throw Socket::error("receive: connection refused");
        for (int i = 0; i < size; i++)
            message[i + received] = buffer[i];
        received += size;
    }
    return received;
}

std::string Socket::receive() {
    std::string buffer;
    auto offset = -1;
    auto index = std::string::npos;
    while (index == std::string::npos) {
        char buf[BUFFER_SIZE];
        int length = raw_receive(descriptor, buf, BUFFER_SIZE, 0);
        std::string data(buf, static_cast<unsigned long>(length));
        buffer.append(data);
        index = data.find("\r\n");
        offset++;
    }
    std::string message = buffer.substr(0, offset * BUFFER_SIZE + index);
    return message;
}

void Socket::send(const char *message) {
    std::string data(message);
    data.append("\r\n", 2);
    for (unsigned left = 0; left < data.length(); left += BUFFER_SIZE) {
        auto right = left + BUFFER_SIZE;
        auto sub_data = data.substr(left, right);
        auto length = ::send(descriptor, sub_data.c_str(), BUFFER_SIZE, 0);
        if (length <= 0) throw Socket::error("send: connection refused");
    }
}

void Socket::send(const std::string &message) {
    send(message.c_str());
}

int Socket::raw_close() {
    return ::close(descriptor);
}

void Socket::close() {
    auto success = raw_close();
    if (success < 0) throw Socket::error("close: error");
}


socket_t Socket::accept() {
    auto socket = ::accept(descriptor, nullptr, nullptr);
    if (socket < 0) throw Socket::error("accept: error");
    return socket;
}

address_t Socket::get_address() const {
    address_t address{};
    auto *flatten = reinterpret_cast<sockaddr *>(&address);
    socklen_t address_len = sizeof(address);
    auto success = ::getsockname(descriptor, flatten, &address_len);
    if (success < 0) throw Socket::error("address: error");
    return address;
}

bool Socket::closed() const {
    address_t address{};
    auto *flatten = reinterpret_cast<sockaddr *>(&address);
    socklen_t address_len = sizeof(address);
    auto success = ::getsockname(descriptor, flatten, &address_len);
    return success >= 0;
}

address_t Socket::address(const std::string &host, uint16_t port) {
    return address(host.c_str(), port);
}

address_t Socket::address(const char *host, uint16_t port) {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);
    return address;
}

address_t Socket::address(uint16_t port) {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    return address;
}

void Socket::shutdown() {
    auto &&success = raw_shutdown();
    if (success < 0) throw Socket::error("shutdown: error");
}

int Socket::raw_shutdown() {
    return ::shutdown(descriptor, SHUT_RDWR);
}

bool Socket::select() {
    return select(nullptr);
}

bool Socket::select(timeval *timeout) {
    fd_set view{};
    FD_ZERO(&view);
    FD_SET(descriptor, &view);
    auto &&ready = ::select(descriptor + 1, &view, nullptr, nullptr, timeout);
    if (ready < 0) throw Socket::error("select is ripped");
    return static_cast<bool>(ready);
}

void Socket::set_options(int option) {
    int one = 1;
    ::setsockopt(descriptor, SOL_SOCKET, option, &one, sizeof(int));
}

std::ostream &operator<<(std::ostream &stream, const Socket &socket) {
    auto &&address = socket.get_address();
    auto &&host = inet_ntoa(address.sin_addr);
    auto &&port = address.sin_port;
    stream << socket.descriptor << " " << host << ":" << port;
    return stream;
}


#include <iostream>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Socket.h"

Socket::Socket(int domain, int type, int protocol) : descriptor(::socket(domain, type, protocol)) {}

Socket::Socket(socket_t socket) : descriptor(socket) {}

Socket::~Socket() = default;

void Socket::bind(const address_t &address) {
    int success = ::bind(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0) throw Socket::error("bind: address already used");
}

void Socket::listen(int backlog) {
    int success = ::listen(descriptor, backlog);
    if (success < 0) throw Socket::error("listen: error");
}

void Socket::connect(const address_t &address) {
    int success = ::connect(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0) throw Socket::error("connect: connection hast'n exist");
}

int raw_receive(int socket, char *message, size_t length, int flags) {
    int received = 0;
    while (received < length) {
        char buffer[length];
        ssize_t size = ::recv(socket, buffer, length, flags);
        if (size <= 0) throw Socket::error("receive: connection refused");
        for (int i = 0; i < size; i++)
            message[i + received] = buffer[i];
        received += size;
    }
    return received;
}

std::string Socket::receive(int flags) {
    std::string buffer;
    auto offset = -1;
    auto index = std::string::npos;
    while (index == std::string::npos) {
        char buf[BUFFER_SIZE];
        int length = raw_receive(descriptor, buf, BUFFER_SIZE, flags);
        std::string data(buf, static_cast<unsigned long>(length));
        buffer.append(data);
        index = data.find("\r\n");
        offset++;
    }
    std::string message = buffer.substr(0, offset * BUFFER_SIZE + index);
    return message;
}

void Socket::send(const char *message, int flags) {
    std::string data(message);
    data.append("\r\n", 2);
    for (unsigned left = 0; left < data.length(); left += BUFFER_SIZE) {
        auto right = left + BUFFER_SIZE;
        std::string sub_data = data.substr(left, right);
        ssize_t length = ::send(descriptor, sub_data.c_str(), BUFFER_SIZE, flags);
        if (length <= 0) throw Socket::error("send: connection refused");
    }
}

void Socket::send(const std::string &message, int flags) {
    send(message.c_str(), flags);
}

int Socket::raw_close() {
    return ::close(descriptor);
}

void Socket::close() {
    int success = raw_close();
    if (success < 0) throw Socket::error("close: error");
}

socket_t Socket::accept() {
    int sock = ::accept(descriptor, nullptr, nullptr);
    if (sock < 0) throw Socket::error("accept: error");
    return sock;
}


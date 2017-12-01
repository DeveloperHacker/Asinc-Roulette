#include "LinuxSocket.h"
#include <algorithm>
#include <unistd.h>

LinuxSocket::LinuxSocket(int domain, int type, int protocol) : descriptor(::socket(domain, type, protocol)) {}

LinuxSocket::LinuxSocket(socket_t socket) : descriptor(socket) {}

LinuxSocket::~LinuxSocket() = default;

void LinuxSocket::bind(const address_t &address) {
    auto success = ::bind(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0) throw LinuxSocket::error("bind: address already used");
}

void LinuxSocket::listen(int backlog) {
    auto success = ::listen(descriptor, backlog);
    if (success < 0) throw LinuxSocket::error("listen: error");
}

void LinuxSocket::connect(const address_t &address) {
    auto success = ::connect(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0) throw LinuxSocket::error("connect: connection hast'n exist");
}

int raw_receive(socket_t socket, char *message, int flags) {
    int received = 0;
    while (received < LinuxSocket::BUFFER_SIZE) {
        char buffer[LinuxSocket::BUFFER_SIZE];
        auto size = ::recv(socket, buffer, LinuxSocket::BUFFER_SIZE, flags);
        if (size <= 0)
            throw LinuxSocket::error("receive: connection refused");
        for (int i = 0; i < size; i++)
            message[i + received] = buffer[i];
        received += size;
    }
    return received;
}

std::string LinuxSocket::receive() {
    std::string buffer;
    auto offset = -1;
    auto index = std::string::npos;
    while (index == std::string::npos) {
        char buf[BUFFER_SIZE];
        int length = raw_receive(descriptor, buf, 0);
        std::string data(buf, static_cast<unsigned long>(length));
        buffer.append(data);
        index = data.find("\r\n");
        offset++;
    }
    std::string message = buffer.substr(0, offset * BUFFER_SIZE + index);
    return message;
}

void LinuxSocket::send(const char *message) {
    std::string data(message);
    send(data);
}

void LinuxSocket::send(std::string message) {
    message.append("\r\n", 2);
    for (unsigned left = 0; left < message.length(); left += BUFFER_SIZE) {
        auto right = left + BUFFER_SIZE;
        auto sub_data = message.substr(left, right);
        auto length = ::send(descriptor, sub_data.c_str(), BUFFER_SIZE, 0);
        if (length <= 0)
            throw LinuxSocket::error("send: connection refused");
    }
}

int LinuxSocket::safe_close() {
    return ::close(descriptor);
}

void LinuxSocket::close() {
    auto success = safe_close();
    if (success < 0)
        throw LinuxSocket::error("close: error");
}


socket_t LinuxSocket::accept() {
    auto socket = ::accept(descriptor, nullptr, nullptr);
    if (socket < 0)
        throw LinuxSocket::error("accept: error");
    return socket;
}

address_t LinuxSocket::get_address() const {
    address_t address{};
    auto *flatten = reinterpret_cast<sockaddr *>(&address);
    socklen_t address_len = sizeof(address);
    auto success = ::getsockname(descriptor, flatten, &address_len);
    if (success < 0) throw LinuxSocket::error("address: error");
    return address;
}

address_t LinuxSocket::address(const std::string &host, uint16_t port) {
    return address(host.c_str(), port);
}

address_t LinuxSocket::address(const char *host, uint16_t port) {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);
    return address;
}

address_t LinuxSocket::address(uint16_t port) {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    return address;
}

void LinuxSocket::shutdown() {
    auto &&success = safe_shutdown();
    if (success < 0) throw LinuxSocket::error("shutdown: error");
}

int LinuxSocket::safe_shutdown() {
    return ::shutdown(descriptor, SHUT_RDWR);
}

bool LinuxSocket::select() {
    return select(nullptr);
}

bool LinuxSocket::select(timeval *timeout) {
    fd_set view{};
    FD_ZERO(&view);
    FD_SET(descriptor, &view);
    auto &&ready = ::select(descriptor + 1, &view, nullptr, nullptr, timeout);
    if (ready < 0) throw LinuxSocket::error("select is ripped");
    return static_cast<bool>(ready);
}

void LinuxSocket::set_options(int option) {
    int one = 1;
    ::setsockopt(descriptor, SOL_SOCKET, option, &one, sizeof(int));
}

std::ostream &operator<<(std::ostream &stream, const LinuxSocket &socket) {
    auto &&address = socket.get_address();
    return stream << address;
}

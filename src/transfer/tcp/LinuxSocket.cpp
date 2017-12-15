#include "LinuxSocket.h"
#include <algorithm>
#include <unistd.h>

const std::string LinuxSocket::DELIMITER("\r\n");

LinuxSocket::LinuxSocket() : descriptor(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) {}

LinuxSocket::LinuxSocket(socket_t descriptor) : descriptor(descriptor) {}

LinuxSocket::~LinuxSocket() = default;

void LinuxSocket::bind(const address_t &address) {
    auto success = ::bind(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0)
        throw error("bind: address already used");
}

void LinuxSocket::listen(int backlog) {
    auto success = ::listen(descriptor, backlog);
    if (success < 0)
        throw error("listen: error");
}

void LinuxSocket::connect(const address_t &address) {
    auto success = ::connect(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0)
        throw error("connect: connection hast'n exist");
}

size_t LinuxSocket::receive(char *message, int flags) {
    auto size = ::recv(descriptor, message, BUFFER_SIZE, flags);
    if (size <= 0)
        throw error("receive: connection refused");
    return static_cast<size_t>(size);
}

std::string LinuxSocket::receive() {
    if (received.empty())
        throw error("receive: socket is empty");
    auto message = received.front();
    received.pop();
    return message;
}

void LinuxSocket::send(const char *message) {
    std::string data(message);
    send(data);
}

void LinuxSocket::send(std::string message) {
    message.append(DELIMITER);
    for (unsigned left = 0; left < message.length(); left += BUFFER_SIZE) {
        auto right = std::min(left + BUFFER_SIZE, message.length());
        auto sub_data = message.substr(left, right);
        auto length = ::send(descriptor, sub_data.c_str(), right - left, 0);
        if (length <= 0)
            throw error("send: connection refused");
    }
}

void LinuxSocket::close() {
    ::close(descriptor);
}

std::shared_ptr<LinuxSocket> LinuxSocket::accept() {
    auto socket = ::accept(descriptor, nullptr, nullptr);
    if (socket < 0)
        throw error("accept: error");
    return std::make_shared<LinuxSocket>(socket);
}

std::shared_ptr<address_t> LinuxSocket::get_address() const {
    auto &&address = std::make_shared<address_t>();
    auto *flatten = reinterpret_cast<sockaddr *>(address.get());
    socklen_t address_len = sizeof(address);
    auto success = ::getsockname(descriptor, flatten, &address_len);
    if (success < 0)
        return this->address;
    this->address = address;
    return address;
}

void LinuxSocket::shutdown() {
    ::shutdown(descriptor, SHUT_RDWR);
}

bool LinuxSocket::select(timeval *timeout) {
    fd_set view{};
    FD_ZERO(&view);
    FD_SET(descriptor, &view);
    auto &&ready = ::select(descriptor + 1, &view, nullptr, nullptr, timeout);
    if (ready < 0)
        throw error("select is ripped");
    return static_cast<bool>(ready);
}

void LinuxSocket::set_options(int option) {
    int one = 1;
    ::setsockopt(descriptor, SOL_SOCKET, option, &one, sizeof(int));
}

void LinuxSocket::update() {
    char data[BUFFER_SIZE];
    auto length = receive(data, 0);
    buffer.append(data, length);
    auto index = buffer.find(DELIMITER);
    while (index != std::string::npos) {
        auto message = buffer.substr(0, index);
        buffer = buffer.substr(index + DELIMITER.length());
        received.push(message);
        index = buffer.find(DELIMITER);
    }
}

address_t LinuxSocket::make_address(const std::string &host, uint16_t port) {
    return make_address(host.c_str(), port);
}

address_t LinuxSocket::make_address(const char *host, uint16_t port) {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);
    return address;
}

address_t LinuxSocket::make_address(uint16_t port) {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    return address;
}

socket_t LinuxSocket::get_descriptor() {
    return descriptor;
}

bool LinuxSocket::empty() {
    return received.empty();
}

std::ostream &LinuxSocket::write(std::ostream &stream, std::shared_ptr<address_t> address) {
    if (address == nullptr)
        return stream << "<invalid-address>";
    else
        return stream << *address;
}

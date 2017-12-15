#include "LinuxUDPSocket.h"

LinuxUDPSocket::LinuxUDPSocket() : descriptor(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {}

LinuxUDPSocket::LinuxUDPSocket(socket_t descriptor) : descriptor(descriptor) {}

LinuxUDPSocket::~LinuxUDPSocket() = default;

socket_t LinuxUDPSocket::get_descriptor() {
    return descriptor;
}

bool LinuxUDPSocket::select(timeval *timeout) {
    fd_set view{};
    FD_ZERO(&view);
    FD_SET(descriptor, &view);
    auto &&ready = ::select(descriptor + 1, &view, nullptr, nullptr, timeout);
    if (ready < 0)
        throw error("select is ripped");
    return static_cast<bool>(ready);
}

std::shared_ptr<address_t> LinuxUDPSocket::get_address() const {
    auto &&address = std::make_shared<address_t>();
    auto *flatten = reinterpret_cast<sockaddr *>(address.get());
    socklen_t address_len = sizeof(address);
    auto success = ::getsockname(descriptor, flatten, &address_len);
    if (success < 0)
        return this->address;
    return address;
}

std::shared_ptr<Socket> LinuxUDPSocket::accept() {
    return std::shared_ptr<Socket>();
}

void LinuxUDPSocket::bind(const address_t &address) {

}

void LinuxUDPSocket::set_options(int option) {

}

void LinuxUDPSocket::listen(int backlog) {

}

void LinuxUDPSocket::connect(const address_t &address) {

}

void LinuxUDPSocket::send(const char *message) {

}

void LinuxUDPSocket::send(std::string message) {

}

std::string LinuxUDPSocket::receive() {
    return std::__cxx11::string();
}

void LinuxUDPSocket::close() {

}

void LinuxUDPSocket::shutdown() {

}

bool LinuxUDPSocket::empty() {
    return false;
}

void LinuxUDPSocket::update(Socket::Event event) {

}

address_t LinuxUDPSocket::make_address(const std::string &host, uint16_t port) {
    return address_t();
}

address_t LinuxUDPSocket::make_address(const char *host, uint16_t port) {
    return address_t();
}

address_t LinuxUDPSocket::make_address(uint16_t port) {
    return address_t();
}

size_t LinuxUDPSocket::receive(char *message, int flags) {
    return 0;
}

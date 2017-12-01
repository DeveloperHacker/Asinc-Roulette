#include "WinSocket.h"

socket_t init(int domain, int type, int protocol) {
    WSADATA wsa_data{0};

    auto &&startup_status = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (startup_status != 0)
        throw WinSocket::error("WSAStartup failed with error code " + std::to_string(startup_status));

    socket_t descriptor = ::socket(domain, type, protocol);
    if (descriptor == INVALID_SOCKET) {
        WSACleanup();
        throw WinSocket::error("Socket creation failed with status " + std::to_string(WSAGetLastError()));
    }
    return descriptor;
}

WinSocket::WinSocket(int domain, int type, int protocol) : descriptor(init(domain, type, protocol)) {}

WinSocket::WinSocket(socket_t socket) : descriptor(socket) {
    // TODO(sergei): init socket address info
}

WinSocket::~WinSocket() {
    WSACleanup();
    if (address_info != nullptr)
        freeaddrinfo(address_info);
}

void WinSocket::connect(const address_t &address) {
    auto &&connect_status = ::connect(descriptor, address->ai_addr, static_cast<int>(address->ai_addrlen));
    address_info = address;
    if (connect_status == SOCKET_ERROR) {
        throw WinSocket::error("Connection to server failed with status " + std::to_string(WSAGetLastError()));
    }
}

int WinSocket::safe_shutdown() {
    return closesocket(descriptor);
}

void WinSocket::shutdown() {
    auto &&success = safe_shutdown();
    if (success < 0)
        throw WinSocket::error("shutdown: error");
}

int WinSocket::safe_close() {
    return ::closesocket(descriptor);
}

void WinSocket::close() {
    auto success = safe_close();
    if (success < 0)
        throw WinSocket::error("close: error");
}

int raw_receive(socket_t socket, char *message, int flags) {
    int received = 0;
    while (received < WinSocket::BUFFER_SIZE) {
        char buffer[WinSocket::BUFFER_SIZE];
        auto size = ::recv(socket, buffer, WinSocket::BUFFER_SIZE, flags);
        if (size <= 0)
            throw WinSocket::error("receive: connection refused");
        for (int i = 0; i < size; i++)
            message[i + received] = buffer[i];
        received += size;
    }
    return received;
}

std::string WinSocket::receive() {
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

void WinSocket::send(const char *message) {
    std::string data(message);
    send(data);
}

void WinSocket::send(std::string message) {
    message.append("\r\n", 2);
    for (unsigned left = 0; left < message.length(); left += BUFFER_SIZE) {
        auto right = left + BUFFER_SIZE;
        auto sub_data = message.substr(left, right);
        auto length = ::send(descriptor, sub_data.c_str(), BUFFER_SIZE, 0);
        if (length <= 0)
            throw WinSocket::error("send: connection refused");
    }
}

void WinSocket::listen(int backlog) {
    throw WinSocket::error("unsupported operation, listen");
}

void WinSocket::set_options(int option) {
    throw WinSocket::error("unsupported operation, set_options");
}

void WinSocket::bind(const address_t &address) {
    throw WinSocket::error("unsupported operation, bind");
}

socket_t WinSocket::accept() {
    throw WinSocket::error("unsupported operation, accept");
}

address_t WinSocket::get_address() const {
    return address_info;
}

bool WinSocket::select(timeval *timeout) {
    throw WinSocket::error("unsupported operation, select with timeout");
}

bool WinSocket::select() {
    throw WinSocket::error("unsupported operation, select");
}

address_t address(int domain, int type, int protocol, const char *host, const char *port) {
    addrinfo hints{};
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = domain;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    address_t address_info = nullptr;

    auto &&getaddr_status = getaddrinfo(host, port, &hints, &address_info);
    if (getaddr_status != 0) {
        throw WinSocket::error("Getaddr failed with status " + std::to_string(getaddr_status));
    }
    return address_info;
}

address_t WinSocket::address(int domain, int type, int protocol, const std::string &host, uint16_t port) {
    return address(domain, type, protocol, host.c_str(), port);
}

address_t WinSocket::address(int domain, int type, int protocol, const char *host, uint16_t port) {
    auto &&win_port = std::to_string((int) port).c_str();
    return ::address(domain, type, protocol, host, win_port);
}

address_t WinSocket::address(int domain, int type, int protocol, uint16_t port) {
    return address(domain, type, protocol, INADDR_ANY, port);
}

std::ostream &operator<<(std::ostream &stream, const WinSocket &socket) {
    auto &&address = socket.get_address();
    return stream << address;
}
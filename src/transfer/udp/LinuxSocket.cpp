#include "LinuxSocket.h"
#include "../../session/strings.h"

const std::string LinuxSocket::DELIMITER("\r\n");

LinuxSocket::LinuxSocket() :
        descriptor(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)),
        retry_timeout(0),
        disconnect_timeout(0) {}

LinuxSocket::LinuxSocket(socket_t descriptor) :
        descriptor(descriptor),
        retry_timeout(0),
        disconnect_timeout(0) {}

LinuxSocket::~LinuxSocket() = default;

socket_t LinuxSocket::get_descriptor() {
    return descriptor;
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

std::shared_ptr<address_t> LinuxSocket::get_address() const {
    auto &&address = std::make_shared<address_t>();
    auto *flatten = reinterpret_cast<sockaddr *>(address.get());
    socklen_t address_len = sizeof(address);
    auto success = ::getsockname(descriptor, flatten, &address_len);
    if (success < 0)
        return this->address;
    return address;
}

std::shared_ptr<LinuxSocket> LinuxSocket::accept(address_t address) {
    auto &&socket = std::make_shared<LinuxSocket>(descriptor);
    socket->address = std::make_shared<address_t>(address);
    return socket;
}

void LinuxSocket::bind(const address_t &address) {
    auto success = ::bind(descriptor, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (success < 0)
        throw error("bind: address already used");
}

void LinuxSocket::set_options(int option) {
    int one = 1;
    ::setsockopt(descriptor, SOL_SOCKET, option, &one, sizeof(int));
}

void LinuxSocket::listen(int backlog) {
    // nothing
}

void LinuxSocket::connect(const address_t &address) {
    this->address = std::make_shared<address_t>(address);
}

void LinuxSocket::send(const char *message) {
    std::string data(message);
    send(data);
}

void LinuxSocket::send(std::string message) {
    buffered_send(message);
}


void LinuxSocket::raw_send(const std::string &message) {
    auto addr = reinterpret_cast<sockaddr *>(address.get());
    auto addr_len = sizeof(*addr);
    auto length = ::sendto(descriptor, message.c_str(), message.length(), 0, &*addr, addr_len);
    if (length <= 0)
        throw error("send: connection refused");
}

void LinuxSocket::raw_send(char type) {
    std::string message;
    message.push_back(type);
    raw_send(message);
}

void LinuxSocket::buffered_send(std::string message) {
    message.append(DELIMITER);
    auto message_size = BUFFER_SIZE - HEAD;
    if (message.length() >= MAX_FRAGMENTATION_POWER * message_size)
        throw LinuxSocket::error("send message is too big");
    disconnect_timeout = 0;
    auto package_numbers = (message.length() / message_size) + 1;
    std::vector<std::string> buffer;
    for (auto package_number = 0; package_number < package_numbers; ++package_number) {
        auto left = package_number * message_size;
        auto right = std::min(left + message_size, message.length());
        std::string send_data;
        send_data.push_back(USER);
        send_data.push_back(package_id);
        send_data.push_back(static_cast<char>(package_number));
        send_data.push_back(static_cast<char>(package_numbers));
        send_data.append(message.substr(left, right));
        buffer.push_back(send_data);
        raw_send(send_data);
    }
    package_id++;
    send_buffer.push_back(buffer);
}

std::string LinuxSocket::receive() {
    if (receive_buffer.empty())
        throw error("receive: socket is empty");
    auto message = receive_buffer.front();
    receive_buffer.pop();
    return message;
}

void LinuxSocket::close() {
    raw_send(EXIT);
}

bool LinuxSocket::empty() {
    return receive_buffer.empty();
}

std::tuple<size_t, address_t> LinuxSocket::raw_receive(char *message, int flags) {
    address_t address{};
    auto addr = reinterpret_cast<sockaddr *>(&address);
    socklen_t addr_size = sizeof(address);
    auto size = ::recvfrom(descriptor, message, BUFFER_SIZE, flags, addr, &addr_size);
    if (size <= 0)
        throw error("receive: connection refused");
    return std::make_tuple<size_t, address_t>(static_cast<size_t>(size), std::move(address));
}

std::tuple<std::string, address_t> LinuxSocket::update() {
    char data[BUFFER_SIZE];
    auto &&result = raw_receive(data, 0);
    auto length = std::get<0>(result);
    auto address = std::get<1>(result);
    std::string message(data, length);
    return std::make_tuple<std::string, address_t>(std::move(message), std::move(address));
}

bool LinuxSocket::update(size_t timeout) {
    disconnect_timeout += timeout;
    retry_timeout += timeout;
    if (disconnect_timeout > DISCONNECT_TIMEOUT)
        return true;
    if (retry_timeout > RETRY_TIMEOUT) {
        for (auto &&messages: send_buffer)
            for (auto &&message: messages)
                raw_send(message);
        retry_timeout = 0;
    }
    return false;
}

bool LinuxSocket::update(const std::string &data) {
    disconnect_timeout = 0;
    if (data.length() > BUFFER_SIZE)
        throw LinuxSocket::error("update error: message is too big");
    if (data.length() < 1)
        throw LinuxSocket::error("update error: message is too small");
    auto &&package_type = data.c_str()[0];
    if (package_type == EXIT)
        return true;
    if (package_type == SUCCESS) {
        if (send_buffer.empty())
            return false;
        send_buffer.pop_front();
        retry_timeout = 0;
        return false;
    }
    if (package_type != USER)
        throw LinuxSocket::error("update error: undefined package type");
    if (data.length() < HEAD)
        throw LinuxSocket::error("update error: user message is too small");
    auto &&package_id = data.c_str()[1];
    auto &&package_number = data.c_str()[2];
    auto &&package_numbers = data.c_str()[3];
    if (package_id != expected_id)
        return false;
    if (package_number != expected_number)
        return false;
    if (expected_numbers != WAIT_PACKAGE && package_numbers != expected_numbers)
        return false;
    expected_numbers = package_numbers;
    expected_number++;
    if (expected_number == expected_numbers) {
        expected_numbers = WAIT_PACKAGE;
        expected_number = 0;
        expected_id++;
        raw_send(SUCCESS);
    }
    retry_timeout = 0;
    buffer.append(std::begin(data) + HEAD, std::end(data));
    auto index = buffer.find(DELIMITER);
    while (index != std::string::npos) {
        auto message = buffer.substr(0, index);
        buffer = buffer.substr(index + DELIMITER.length());
        receive_buffer.push(message);
        index = buffer.find(DELIMITER);
    }
    return false;
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

std::ostream &LinuxSocket::write(std::ostream &stream, std::shared_ptr<address_t> address) {
    if (address == nullptr)
        return stream << "<invalid-address>";
    else
        return stream << *address;
}

uint64_t LinuxSocket::concat(address_t address) {
    return address.sin_port << sizeof(in_addr_t) | address.sin_addr.s_addr;
}

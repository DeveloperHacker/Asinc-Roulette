#pragma once

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <queue>
#include <memory>

using socket_t = int;
using address_t = sockaddr_in;

class LinuxSocket {
public:
    class error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;

        ~error() override = default;
    };

public:
    const static size_t BUFFER_SIZE = 512;
    const static std::string DELIMITER;
    const static int SOCKET_ERROR = -1;

private:
    const socket_t descriptor;
    std::string buffer;
    std::queue<std::string> received;
    address_t address;

public:
    LinuxSocket();

    explicit LinuxSocket(socket_t descriptor);

    ~LinuxSocket();

    socket_t get_descriptor();

    bool select(timeval *timeout);

    address_t get_address() const;

    std::shared_ptr<LinuxSocket> accept();

    void bind(const address_t &address);

    void set_options(int option);

    void listen(int backlog);

    void connect(const address_t &address);

    void send(const char *message);

    void send(std::string message);

    std::string receive();

    void close();

    void shutdown();

    bool empty();

    void update();

public:
    static address_t make_address(const std::string &host, uint16_t port);

    static address_t make_address(const char *host, uint16_t port);

    static address_t make_address(uint16_t port);

    friend std::ostream &operator<<(std::ostream &stream, std::shared_ptr<LinuxSocket> socket);

private:
    size_t receive(char *message, int flags);
};

static std::ostream &operator<<(std::ostream &stream, address_t address) {
    auto &&host = inet_ntoa(address.sin_addr);
    auto &&port = address.sin_port;
    return stream << host << ":" << port;
}

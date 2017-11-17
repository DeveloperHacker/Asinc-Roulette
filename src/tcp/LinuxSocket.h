#pragma once


#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <stdexcept>
#include <iomanip>

typedef int socket_t;
typedef sockaddr_in address_t;

class LinuxSocket {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

public:
    const static size_t BUFFER_SIZE = 512;
    const socket_t descriptor;

public:
    LinuxSocket(int domain, int type, int protocol);

    explicit LinuxSocket(socket_t socket);

    ~LinuxSocket();

    bool select();

    bool select(timeval *timeout);

    address_t get_address() const;

    socket_t accept();

    void bind(const address_t &address);

    void set_options(int option);

    void listen(int backlog);

    void connect(const address_t &address);

    void send(const char *message);

    void send(std::string message);

    std::string receive();

    void close();

    int safe_close();

    void shutdown();

    int safe_shutdown();

    static address_t address(const std::string &host, uint16_t port);

    static address_t address(const char *host, uint16_t port);

    static address_t address(uint16_t port);

    friend std::ostream &operator<<(std::ostream &stream, const LinuxSocket &socket);
};

static std::ostream &operator<<(std::ostream &stream, address_t address) {
    auto &&host = inet_ntoa(address.sin_addr);
    auto &&port = address.sin_port;
    return stream << host << ":" << port;
}

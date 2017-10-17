#pragma once

#include <netinet/in.h>
#include <string>
#include <stdexcept>

typedef int socket_t;
typedef sockaddr_in address_t;

class Socket {
public:
    class error;

public:
    const static size_t BUFFER_SIZE = 512;
    const socket_t descriptor;

public:
    Socket(int domain, int type, int protocol);

    explicit Socket(socket_t socket);

    ~Socket();

    bool select();

    bool select(timeval *timeout);

    address_t get_address();

    socket_t accept();

    void bind(const address_t &address);

    void listen(int backlog);

    void connect(const address_t &address);

    void send(const char *message);

    void send(const std::string &message);

    std::string receive();

    void close();

    int raw_close();

    void shutdown();

    int raw_shutdown();

public:
    static address_t address(const std::string &host, uint16_t port);

    static address_t address(const char *host, uint16_t port);

    static address_t address(uint16_t port);
};

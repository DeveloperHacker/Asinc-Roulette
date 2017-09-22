#pragma once

#include <netinet/in.h>
#include <string>
#include <stdexcept>

typedef int socket_t;
typedef sockaddr_in address_t;

class Socket {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;

    public:
        virtual ~error() = default;
    };

public:
    const static size_t BUFFER_SIZE = 512;
    const socket_t descriptor;

public:
    Socket(int domain, int type, int protocol);

    explicit Socket(socket_t socket);

    ~Socket();

    socket_t accept();

    void bind(const address_t &address);

    void listen(int backlog);

    void connect(const address_t &address);

    void send(const char *message, int flags);

    void send(const std::string &message, int flags);

    std::string receive(int flags);

    void close();

    int raw_close();
};


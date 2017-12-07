#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

typedef addrinfo *address_t;
typedef SOCKET socket_t;

class WinSocket {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

public:
    const static size_t BUFFER_SIZE = 512;
    const socket_t descriptor;

private:
    address_t address_info = nullptr;

public:
    WinSocket(int domain, int type, int protocol);

    explicit WinSocket(socket_t socket);

    ~WinSocket();

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

    static void startup();

    static void cleanup();

    static address_t address(int domain, int type, int protocol, const std::string &host, uint16_t port);

    static address_t address(int domain, int type, int protocol, const char *host, uint16_t port);

    static address_t address(int domain, int type, int protocol, uint16_t port);

    friend std::ostream &operator<<(std::ostream &stream, const WinSocket &socket);
};

static std::ostream &operator<<(std::ostream &stream, address_t address) {
    throw WinSocket::error("unsupported operation, address is null");
}

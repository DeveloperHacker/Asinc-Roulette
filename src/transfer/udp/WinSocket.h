#pragma once

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <queue>
#include <memory>

typedef addrinfo *address_t;
typedef SOCKET socket_t;

class WinSocket {
public:
    class error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;

        ~error() override = default;
    };

public:
    const static size_t HEAD = 4;
    const static int WAIT_PACKAGE = 0;
    const static size_t BUFFER_SIZE = 512;
    const static size_t MAX_FRAGMENTATION_POWER = 256;
    const static size_t DISCONNECT_TIMEOUT = 60 * 1000;
    const static size_t RETRY_TIMEOUT = 1000;
    const static std::string DELIMITER;
    const static char SUCCESS = 'S';
    const static char USER = 'U';
    const static char EXIT = 'E';

private:
    const socket_t descriptor;
    std::string buffer;
    std::deque<std::vector<std::string>> send_buffer;
    std::queue<std::string> receive_buffer;
    std::shared_ptr<address_t> address;
    uint64_t disconnect_timeout;
    uint64_t retry_timeout;
    char package_id = 0;
    char expected_id = 0;
    char expected_number = 0;
    char expected_numbers = WAIT_PACKAGE;

public:
    WinSocket();

    explicit WinSocket(socket_t descriptor);

    ~WinSocket();

    socket_t get_descriptor();

    bool select(timeval *timeout);

    std::shared_ptr<address_t> get_address() const;

    std::shared_ptr<WinSocket> accept(address_t address);

    void bind(const address_t &address);

    void set_options(int option);

    void listen(int backlog);

    void connect(const address_t &address);

    void send(const char *message);

    void send(std::string message);

    std::string receive();

    void close();

    bool empty();

    std::tuple<std::string, address_t> update();

    bool update(size_t timeout);

    bool update(const std::string &message);

public:
    static void startup();

    static void cleanup();

    static address_t make_address(const std::string &host, uint16_t port);

    static address_t make_address(const char *host, uint16_t port);

    static address_t make_address(uint16_t port);

    static uint64_t concat(address_t address);

    static std::ostream &write(std::ostream &stream, std::shared_ptr<address_t> address);

    friend std::ostream &operator<<(std::ostream &stream, std::shared_ptr<WinSocket> socket) {
        auto &&address = socket->get_address();
        return write(stream, address);
    }

private:
    void raw_send(char type);
    
    void raw_send(const std::string &message);
    
    void buffered_send(std::string message);

    std::tuple<size_t, address_t> raw_receive(char *message, int flags);
};

static std::ostream &operator<<(std::ostream &stream, address_t address) {
    auto addr = reinterpret_cast<sockaddr_in *>(address->ai_addr);
    auto &&host = inet_ntoa(addr->sin_addr);
    auto &&port = addr->sin_port;
    return stream << host << ":" << port;
}

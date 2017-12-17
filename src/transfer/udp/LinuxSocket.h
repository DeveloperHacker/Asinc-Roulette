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
    enum Event {
        DATA_IN,
        TIMEOUT
    };

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
    const static size_t PING_TIMEOUT = 10 * 1000;
    const static size_t RETRY_TIMEOUT = 1000;
    const static std::string DELIMITER;
    const static char SUCCESS = 'S';
    const static char PING = 'P';
    const static char PONG = 'O';
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
    LinuxSocket();

    explicit LinuxSocket(socket_t descriptor);

    ~LinuxSocket();

    socket_t get_descriptor();

    bool select(timeval *timeout);

    std::shared_ptr<address_t> get_address() const;

    std::shared_ptr<LinuxSocket> accept(address_t address);

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
    static address_t make_address(const std::string &host, uint16_t port);

    static address_t make_address(const char *host, uint16_t port);

    static address_t make_address(uint16_t port);

    static uint64_t concat(address_t address);

    static std::ostream &write(std::ostream &stream, std::shared_ptr<address_t> address);

    friend std::ostream &operator<<(std::ostream &stream, std::shared_ptr<LinuxSocket> socket) {
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
    auto &&host = inet_ntoa(address.sin_addr);
    auto &&port = address.sin_port;
    return stream << host << ":" << port;
}

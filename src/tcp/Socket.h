#pragma once

#include <memory>


#ifdef _WIN32
    using id_t = int;
    using socket_t = SOCKET;
    using address_t = addrinfo*;

    static std::ostream &operator<<(std::ostream &stream, address_t address) {
        throw WinSocket::error("unsupported operation");
    }
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <iostream>

    using socket_t = int;
    using address_t = sockaddr_in;

    static std::ostream &operator<<(std::ostream &stream, address_t address) {
        auto &&host = inet_ntoa(address.sin_addr);
        auto &&port = address.sin_port;
        return stream << host << ":" << port;
    }
#endif

class Socket {
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
    const static size_t BUFFER_SIZE = 512;

public:
    virtual bool select(timeval *timeout) = 0;

    virtual socket_t get_descriptor() = 0;

    virtual std::shared_ptr<address_t> get_address() const = 0;

    virtual std::shared_ptr<Socket> accept() = 0;

    virtual void bind(const address_t &address) = 0;

    virtual void set_options(int option) = 0;

    virtual void listen(int backlog) = 0;

    virtual void connect(const address_t &address) = 0;

    virtual void send(const char *message) = 0;

    virtual void send(std::string message) = 0;

    virtual bool empty() = 0;

    virtual void update(Event event) = 0;

    virtual std::string receive() = 0;

    virtual void close() = 0;

    virtual void shutdown() = 0;

    friend std::ostream &operator<<(std::ostream &stream, std::shared_ptr<Socket> socket) {
        auto &&address = socket->get_address();
        write(stream, address);
    }

    static std::ostream &write(std::ostream &stream, std::shared_ptr<address_t> address) {
        if (address == nullptr)
            return stream << "<invalid-address>";
        else
            return stream << *address;
    }
};

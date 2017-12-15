#pragma once


#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <queue>
#include "../Socket.h"

class LinuxTCPSocket : public Socket {
private:
    const socket_t descriptor;
    std::string buffer;
    std::queue<std::string> received;
    std::shared_ptr<address_t> address;

public:
    LinuxTCPSocket();

    explicit LinuxTCPSocket(socket_t descriptor);

    ~LinuxTCPSocket();

    socket_t get_descriptor() override;

    bool select(timeval *timeout) override;

    std::shared_ptr<address_t> get_address() const override;

    std::shared_ptr<Socket> accept() override;

    void bind(const address_t &address) override;

    void set_options(int option) override;

    void listen(int backlog) override;

    void connect(const address_t &address) override;

    void send(const char *message) override;

    void send(std::string message) override;

    std::string receive() override;

    void close() override;

    void shutdown() override;

    bool empty() override;

    void update(Event event) override;

public:
    static address_t make_address(const std::string &host, uint16_t port);

    static address_t make_address(const char *host, uint16_t port);

    static address_t make_address(uint16_t port);

private:
    size_t receive(char *message, int flags);
};

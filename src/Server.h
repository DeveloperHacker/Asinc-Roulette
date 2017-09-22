#pragma once

#include "../tcp/Socket.h"
#include "../tcp/TCPServer.h"
#include <iostream>

class Server : public TCPServer {
public:
    Server(int domain, int type, int protocol, address_t &address) :
            TCPServer(domain, type, protocol, address) {}

    bool handle(SafeSocket &socket) override {
        std::string message = socket.receive(0);
        std::cout << "received: '" << message << "'" << std::endl;
        socket.send(message, 0);
        return message == "close";
    }

    static address_t address() {
        address_t address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(7777);
        address.sin_addr.s_addr = INADDR_ANY;
        return address;
    }
};

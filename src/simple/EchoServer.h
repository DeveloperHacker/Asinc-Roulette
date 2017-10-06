#pragma once

#include "../../tcp/Socket.h"
#include "../../tcp/TCPServer.h"
#include <iostream>
#include <algorithm>

class EchoServer : public TCPServer {
public:
    EchoServer(int domain, int type, int protocol, address_t &address) :
            TCPServer(domain, type, protocol, address) {}

protected:
    bool handle(SafeSocket &socket) override {
        std::string message = socket.receive();
        std::cout << "received: '" << message << "'" << std::endl;
        std::transform(message.begin(), message.end(), message.begin(), ::toupper);
        socket.send(message);
        return message == "close";
    }
};

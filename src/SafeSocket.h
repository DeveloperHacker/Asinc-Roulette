#pragma once


#include "Socket.h"
#include <string>

class SafeSocket {
private:
    bool empty = false;
    Socket socket;

public:
    explicit SafeSocket(Socket &socket) : socket(socket.descriptor) {};

    explicit SafeSocket(socket_t descriptor) : socket(descriptor) {};

    void send(const char *message, int flags) {
        socket.send(message, flags);
    }

    void send(const std::string &message, int flags) {
        socket.send(message, flags);
    }

    std::string receive(int flags) {
        if (empty) throw Socket::error("socket is already read");
        empty = true;
        return socket.receive(flags);
    }
};

#pragma once


#include "Socket.h"
#include <string>

class SendSocket {
private:
    Socket socket;

public:
    explicit SendSocket(Socket &socket) : socket(socket.descriptor) {}; // NOLINT

    explicit SendSocket(socket_t descriptor) : socket(descriptor) {}; // NOLINT

    void send(const char *message) {
        socket.send(message);
    }

    void send(const std::string &message) {
        socket.send(message);
    }
};

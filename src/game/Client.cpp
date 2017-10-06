#pragma once

#include "Client.h"
#include "../Config.h"

Client::Client(int domain, int type, int protocol, sockaddr_in &address) :
        TCPClient(domain, type, protocol, address) {
}

Client::~Client() {
    delete encoder;
}

bool Client::handle(Socket &socket) {
    switch (state) {
        case INIT :
            return init(socket);
        case WORK :
            return work(socket);
    }
    return false;
}

bool Client::init(Socket &socket) {
    socket.send(Config::command::GET_PUBLIC_KEY);
    std::__cxx11::string public_key = socket.receive();
    encoder = new Encoder(public_key);
    state = WORK;
    return false;
}

bool Client::work(Socket &socket) {
    std::string message("message");
    message = encoder->encode(message);
    socket.send(message);
    return true;
}

#pragma once

#include "../../tcp/Socket.h"
#include "../../tcp/TCPServer.h"
#include "../Transfer.h"
#include <iostream>
#include <algorithm>

class EchoServer : public TCPServer {
public:
    enum State {
        INIT,
        READY
    };

private:
    std::unordered_map<id_t, std::pair<std::shared_ptr<Transfer>, EchoServer::State>> clients;

public:
    EchoServer(int domain, int type, int protocol, address_t &address);

protected:
    bool handle(const std::string &message, id_t id, SendSocket &socket) override;

    void connect_handle(id_t id) override;

    void disconnect_handle(id_t id) override;
};

#pragma once

#include "../../tcp/Socket.h"
#include "../../tcp/TCPServer.h"
#include "../Transfer.h"
#include <iostream>
#include <algorithm>

const std::string INIT_MESSAGE("init");

class EchoServer : public TCPServer {
private:
    Transfer transfer;
    Transfer::State state;

public:
    EchoServer(int domain, int type, int protocol, address_t &address);

protected:
    bool handle(const std::string &message, SendSocket &socket) override;
};

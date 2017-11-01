#pragma once

#include "../../tcp/TCPClient.h"
#include "State.h"

class Client : public TCPClient {
private:
    State state{GUEST};

public:
    Client(int domain, int type, int protocol, address_t &address);

    ~Client();

private:
    virtual void input(const std::string &message);

    virtual std::string output();
};



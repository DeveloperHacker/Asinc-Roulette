#pragma once

#include "../transfer/Socket.h"
#include "../transfer/TransferServer.h"
#include "../../session/Transfer.h"
#include "../../session/sessionServer.h"
#include <iostream>
#include <algorithm>

class Server : public CryptoServer {
public:
    Server(int domain, int type, int protocol, address_t &address);

protected:
    bool crypto_handle(identifier_t id, address_t address, const std::string &message) override;

    bool handle(identifier_t id, address_t address, const std::string &message) override;

    void connect_handle(identifier_t id, address_t address) override;

    void disconnect_handle(identifier_t id, address_t address) override;
};

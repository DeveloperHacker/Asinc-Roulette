#pragma once

#include "../tcp/Socket.h"
#include "../tcp/TransferServer.h"
#include "../../crypto/Transfer.h"
#include "../../crypto/CryptoServer.h"
#include <iostream>
#include <algorithm>

class Server : public CryptoServer {
public:
    Server(int domain, int type, int protocol, address_t &address);

protected:
    bool crypto_handle(id_t id, address_t address, const std::string &message) override;

    bool handle(id_t id, address_t address, const std::string &message) override;

    void connect_handle(id_t id, address_t address) override;

    void disconnect_handle(id_t id, address_t address) override;
};

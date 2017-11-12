#pragma once

#include <vector>
#include "../../tcp/TCPServer.h"
#include "Role.h"
#include "Table.h"


class Server : public TCPServer {
private:
    std::vector<Table> tables{};

public:
    Server(int domain, int type, int protocol, address_t &address);

    bool handle(id_t id, address_t address, const std::string &message) override;

    void connect_handle(id_t id, address_t address) override;

    void disconnect_handle(id_t id, address_t address) override;
};

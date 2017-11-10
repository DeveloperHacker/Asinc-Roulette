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

    bool handle(const std::string &message, id_t id, SendSocket &socket) override;

    void connect_handle(id_t id) override;

    void disconnect_handle(id_t id) override;
};

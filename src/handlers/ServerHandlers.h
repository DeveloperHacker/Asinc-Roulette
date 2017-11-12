#pragma once

#include "Handlers.h"
#include "../../tcp/Socket.h"
#include "../Server.h"

class Server;

class ServerHandlers : public Handlers<bool, Server &, id_t, address_t, const std::string &> {
public:
    ServerHandlers();
};

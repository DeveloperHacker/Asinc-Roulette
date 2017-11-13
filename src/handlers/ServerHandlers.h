#pragma once

#include "Handlers.h"
#include "../../tcp/Socket.h"
#include "../Server.h"
#include "../../lib/json/src/json.hpp"

using json = nlohmann::json;

class Server;

class ServerHandlers : public Handlers<bool, Server &, id_t, address_t, json &> {
public:
    ServerHandlers();
};

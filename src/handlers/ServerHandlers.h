#pragma once

#include "Handlers.h"
#include "../../src/transfer/Socket.h"
#include "../core/Server.h"
#include "json/src/json.hpp"

using json = nlohmann::json;

class Server;

class ServerHandlers : public Handlers<bool, Server &, id_t, json &> {
public:
    ServerHandlers();
};

#pragma once

#include "../../src/simple/SimpleServerCommands.h"
#include "../core/Server.h"

class ServerCommands : public SimpleServerCommands {
public:
    explicit ServerCommands(Server &server) : SimpleServerCommands(server) {}
};
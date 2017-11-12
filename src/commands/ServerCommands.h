#pragma once

#include "../../simple/SimpleServerCommands.h"
#include "../Server.h"

class ServerCommands : public SimpleServerCommands {
public:
    explicit ServerCommands(Server &server) : SimpleServerCommands(server) {}
};
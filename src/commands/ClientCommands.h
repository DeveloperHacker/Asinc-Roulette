#pragma once

#include "../../commands/Commands.h"
#include "../../tcp/TCPClient.h"
#include "../Client.h"

class Client;

class ClientCommands : public Commands {
public:
    ClientCommands();

    void init(Client &client);
};
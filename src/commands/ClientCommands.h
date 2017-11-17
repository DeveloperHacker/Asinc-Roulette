#pragma once

#include "Commands.h"
#include "../../src/tcp/TCPClient.h"
#include "../core/Client.h"

class Client;

class ClientCommands : public Commands {
public:
    ClientCommands();

    void init(Client &client);
};
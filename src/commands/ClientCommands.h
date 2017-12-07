#pragma once

#include "Commands.h"
#include "../tcp/TransferClient.h"
#include "../core/Client.h"

class Client;

class ClientCommands : public Commands {
public:
    ClientCommands();

    void init(Client &client);
};
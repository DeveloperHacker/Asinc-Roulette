#pragma once

#include "Handlers.h"

class Client;

class ClientHandlers : public Handlers<void, Client &, const std::string &> {
public:
    ClientHandlers();

    void execute(permition_t  permition, const std::string &name, Client &client, const std::string &message) override;
};

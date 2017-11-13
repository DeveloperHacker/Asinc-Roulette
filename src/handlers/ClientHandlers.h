#pragma once

#include "Handlers.h"
#include "../../lib/json/src/json.hpp"

using json = nlohmann::json;

class Client;

class ClientHandlers : public Handlers<void, Client &, json &> {
public:
    ClientHandlers();

    void execute(permition_t permition, const std::string &name, Client &client, json &message) override;
};

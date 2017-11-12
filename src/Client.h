#pragma once

#include "../crypto/CryptoClient.h"
#include "../commands/permitions.h"
#include "handlers/ClientHandlers.h"
#include "commands/ClientCommands.h"

class ClientHandlers;

class ClientCommands;

class Client : public CryptoClient {
public:
    permition_t permition;

private:
    std::shared_ptr<ClientHandlers> handlers;
    std::shared_ptr<ClientCommands> commands;

public:
    Client(int domain, int type, int protocol, address_t &address);

    void join() override;

    void login(const std::string &login, const std::string &password);

    void logout();

    void join(const std::string &name, const std::string &password);

    void create(const std::string &name, const std::string &password);

    void leave();

    void write(const std::string &message);

    void tables();

    void users();

    void disconnect();

    void sync();

    void registration(const std::string &login, const std::string &password);

    void set_permition(const std::string &login, permition_t permition);

protected:
    void crypto_output() override;

    void crypto_input(const std::string &message) override;
};

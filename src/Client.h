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
    using CryptoClient::join;

    using CryptoClient::send;

public:
    Client(int domain, int type, int protocol, address_t &address);

    void login(const std::string &login, const std::string &password);

    void logout();

    void join(const std::string &name, const std::string &password);

    void create(const std::string &name, const std::string &password);

    void leave();

    void write(const std::string &message);

    void write(const std::string &login, const std::string &message);

    void tables();

    void users();

    void disconnect();

    void sync();

    void registration(const std::string &login, const std::string &password);

    void set_permition(const std::string &login, permition_t permition);

    void spin();

    void balance();

    void bets();

    void bet(const std::string &type, int number, int value);

private:
    void send(const std::string &command, const json &data);

protected:
    void crypto_output() override;

    void crypto_input(const std::string &message) override;
};

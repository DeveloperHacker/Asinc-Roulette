#pragma once

#include <vector>
#include "../../tcp/TCPServer.h"

enum Role {
    CROUPIER, GAMER
};

using ID = int;

class DataBase {
public:
    static const ID INVALID_ID = -1;

public:
    ID get_id(const std::string &login, const std::string &password);

    Role get_role(ID id);

    ID register_user(const std::string &login, const std::string &password);

    bool set_role(ID id, Role role);

    bool set_role(const std::string &login, Role role);
};

class Table {
private:
    ID croupier;
    std::vector<ID> gamers;

    size_t expect_gamers;


public:
    Table(ID croupier, size_t expect_gamers);

    bool add_user(ID user);

    bool remove_user(ID user);
};

class GameServer : TCPServer {
private:
    std::vector<Table> tables;

public:
    GameServer(int domain, int type, int protocol, address_t &address) : TCPServer(domain, type, protocol, address) {

    }

    bool handle(SafeSocket &socket) override {

    }
};

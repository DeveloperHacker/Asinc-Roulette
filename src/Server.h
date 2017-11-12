#pragma once

#include <unordered_set>
#include "../crypto/CryptoServer.h"
#include "handlers/ServerHandlers.h"
#include "../commands/permitions.h"
#include "core/DataBase.h"

class ServerHandlers;

class Server : public CryptoServer {
public:
    struct session_t {
        std::string name;
        std::string password;
        std::string croupier;
        std::unordered_set<std::string> players;
        size_t max_players;
    };

private:
    DataBase data_base;
    std::shared_ptr<ServerHandlers> handlers;
    std::unordered_map<std::string, id_t> ids;
    std::unordered_map<id_t, std::pair<std::string, permition_t>> auth;
    std::unordered_map<std::string, std::string> croupiers;
    std::unordered_map<std::string, std::string> players;
    std::unordered_map<std::string, session_t> sessions;

public:
    Server(int domain, int type, int protocol, address_t &address);

    void login(id_t id, const std::string &login, const std::string &password);

    void logout(id_t id);

    void join(id_t id, const std::string &name, const std::string &password);

    void create(id_t id, const std::string &name, const std::string &password);

    void leave(id_t id);
    
    void write(id_t id, const std::string &message);
    
    void write(id_t id, const std::string &login, const std::string &message);

    void tables(id_t id);

    void users(id_t id);

    void disconnect(id_t id);

    void sync(id_t id);

    void registration(id_t id, const std::string &login, const std::string &password);

    void set_permition(id_t id, const std::string &login, permition_t permition);

    void leave_croupier(id_t id);

    void leave_player(id_t id);

    std::string get_session_name(id_t id);

    session_t get_session(id_t id);

    std::string get_login(id_t id);

    permition_t get_permition(id_t id);

    id_t get_id(const std::string &login);

    void add_player(id_t id, const std::string &name, const std::string &password);

    void add_croupier(id_t id, const std::string &name, const std::string &password);

    void add_auth(id_t id, const std::string &login, permition_t permition);

    void join() override {
        TCPServer::join();
    }

protected:
    void handle_error(id_t id, const std::string &command, const std::string &message);

    bool crypto_handle(id_t id, address_t address, const std::string &message) override;

    void connect_handle(id_t id, address_t address) override;

    void disconnect_handle(id_t id, address_t address) override;
};

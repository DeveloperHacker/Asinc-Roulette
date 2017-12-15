#pragma once

#include <unordered_set>
#include "../session/SessionServer.h"
#include "../handlers/ServerHandlers.h"
#include "../commands/permissions.h"
#include "../core/DataBase.h"
#include "json/src/json.hpp"

using json = nlohmann::json;

class ServerHandlers;

class Server : public SessionServer {
public:
    struct bet_t {
        std::string type;
        int number;
        money_t value;
    };

    struct session_t {
        std::string name;
        std::string password;
        std::string croupier;
        std::unordered_set<std::string> users;
        size_t max_players;
        std::unordered_map<std::string, std::vector<bet_t>> bets;
    };

private:
    DataBase data_base;
    std::shared_ptr<ServerHandlers> handlers;
    std::unordered_map<identifier_t, std::string> id2name;
    std::unordered_map<std::string, identifier_t> login2id;
    std::unordered_map<identifier_t, DataBase::user_t> users;
    std::unordered_map<std::string, session_t> sessions;

public:
    using SessionServer::join;

    using SessionServer::send;

public:
    Server(std::shared_ptr<Socket> socket);

    void do_signin(identifier_t id, const std::string &login, const std::string &password);

    void do_signout(identifier_t id);

    void do_signup(identifier_t id, const std::string &login, const std::string &password);

    void do_join(identifier_t id, const std::string &name, const std::string &password);

    void do_create(identifier_t id, const std::string &name, const std::string &password);

    void do_leave(identifier_t id);

    void do_write(identifier_t id, const std::string &message);

    void do_write(identifier_t id, const std::string &login, const std::string &message);

    void do_tables(identifier_t id);

    void do_users(identifier_t id);

    void do_disconnect(identifier_t id);

    void do_sync(identifier_t id);

    void do_set_permission(identifier_t id, const std::string &login, permission_t permission);

    void do_balance(identifier_t id);

    void do_bets(identifier_t id);

    void do_bet(identifier_t id, const bet_t &bet);

    void do_spin(identifier_t id);

    void do_kick(identifier_t id, const std::string &login);

    static void validate(const bet_t &bet);

    static bool wining(int random_number, const bet_t &bet);

private:
    void pay(identifier_t id, const bet_t &bet, bool is_positive);

    void local_set_user_permission(identifier_t id, permission_t permission);

    void leave_croupier(identifier_t id);

    void leave_player(identifier_t id);

    std::string get_session_name(identifier_t id);

    session_t &get_session(identifier_t id);

    std::string get_login(identifier_t id);

    permission_t get_permission(identifier_t id);

    identifier_t get_id(const std::string &login);

    void add_player(identifier_t id, const std::string &name, const std::string &password);

    void add_croupier(identifier_t id, const std::string &name, const std::string &password);

    void add_user(identifier_t id, const DataBase::user_t &user);

    void remove_user(identifier_t id);

    DataBase::user_t &get_user(identifier_t id);

    void send(identifier_t id, const std::string &status, const std::string &command, const json &data);

protected:
    void handle_error(identifier_t id, const std::string &command, const std::string &message);

    bool session_handle(identifier_t id, const std::string &message) override;

    void connect_handle(identifier_t id) override;

    void disconnect_handle(identifier_t id) override;
};

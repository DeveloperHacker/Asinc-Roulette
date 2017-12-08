#pragma once

#include <unordered_set>
#include "../crypto/SessionServer.h"
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
    std::unordered_map<id_t, std::string> id2name;
    std::unordered_map<std::string, id_t> login2id;
    std::unordered_map<id_t, DataBase::user_t> users;
    std::unordered_map<std::string, session_t> sessions;

public:
    using SessionServer::join;

    using SessionServer::send;

public:
    Server(std::shared_ptr<Socket> socket);

    void do_signin(id_t id, const std::string &login, const std::string &password);

    void do_signout(id_t id);

    void do_signup(id_t id, const std::string &login, const std::string &password);

    void do_join(id_t id, const std::string &name, const std::string &password);

    void do_create(id_t id, const std::string &name, const std::string &password);

    void do_leave(id_t id);

    void do_write(id_t id, const std::string &message);

    void do_write(id_t id, const std::string &login, const std::string &message);

    void do_tables(id_t id);

    void do_users(id_t id);

    void do_disconnect(id_t id);

    void do_sync(id_t id);

    void do_set_permission(id_t id, const std::string &login, permission_t permission);

    void do_balance(id_t id);

    void do_bets(id_t id);

    void do_bet(id_t id, const bet_t &bet);

    void do_spin(id_t id);

    void do_kick(id_t id, const std::string &login);

    static void validate(const bet_t &bet);

    static bool wining(int random_number, const bet_t &bet);

private:
    void pay(id_t id, const bet_t &bet, bool is_positive);

    void local_set_user_permission(id_t id, permission_t permission);

    void leave_croupier(id_t id);

    void leave_player(id_t id);

    std::string get_session_name(id_t id);

    session_t &get_session(id_t id);

    std::string get_login(id_t id);

    permission_t get_permission(id_t id);

    id_t get_id(const std::string &login);

    void add_player(id_t id, const std::string &name, const std::string &password);

    void add_croupier(id_t id, const std::string &name, const std::string &password);

    void add_user(id_t id, const DataBase::user_t &user);

    void remove_user(id_t id);

    DataBase::user_t &get_user(id_t id);

    void send(id_t id, const std::string &status, const std::string &command, const json &data);

protected:
    void handle_error(id_t id, const std::string &command, const std::string &message);

    bool crypto_handle(id_t id, const std::string &message) override;

    void connect_handle(id_t id) override;

    void disconnect_handle(id_t id) override;
};

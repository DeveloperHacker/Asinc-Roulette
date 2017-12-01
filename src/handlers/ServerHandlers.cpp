#include "ServerHandlers.h"
#include "../core/config.h"

using json = nlohmann::json;

using impl_t = std::function<bool(Server &, id_t, address_t, json &)>;

ServerHandlers::ServerHandlers() : Handlers() {
    impl_t handler_signin = [](Server &server, id_t id, address_t address, json &request) -> bool {
        auto &&login = request[parts::LOGIN];
        auto &&password = request[parts::PASSWORD];
        server.do_signin(id, login, password);
        return false;
    };
    impl_t handler_signout = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_signout(id);
        return false;
    };
    impl_t join = [](Server &server, id_t id, address_t address, json &request) -> bool {
        auto &&name = request[parts::NAME];
        auto &&password = request[parts::PASSWORD];
        server.do_join(id, name, password);
        return false;
    };
    impl_t create = [](Server &server, id_t id, address_t address, json &request) -> bool {
        auto &&name = request[parts::NAME];
        auto &&password = request[parts::PASSWORD];
        server.do_create(id, name, password);
        return false;
    };
    impl_t leave = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_leave(id);
        return false;
    };
    impl_t tables = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_tables(id);
        return false;
    };
    impl_t users = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_users(id);
        return false;
    };
    impl_t write = [](Server &server, id_t id, address_t address, json &request) -> bool {
        auto &&msg = request[parts::MESSAGE];
        if (request.count(parts::LOGIN) > 0) {
            auto &&login = request[parts::LOGIN];
            server.do_write(id, login, msg);
        } else {
            server.do_write(id, msg);
        }
        return false;
    };
    impl_t disconnect = [](Server &server, id_t id, address_t address, json &request) -> bool {
        return true;
    };
    impl_t sync = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_sync(id);
        return false;
    };
    impl_t handler_signup = [](Server &server, id_t id, address_t address, json &request) -> bool {
        auto &&login = request[parts::LOGIN];
        auto &&password = request[parts::PASSWORD];
        server.do_signup(id, login, password);
        return false;
    };
    impl_t set_permission = [](Server &server, id_t id, address_t address, json &request) -> bool {
        auto &&login = request[parts::LOGIN];
        auto &&permission = request[parts::PERMITION];
        server.do_set_permission(id, login, permission);
        return false;
    };
    impl_t command_spin = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_spin(id);
        return false;
    };
    impl_t command_bet = [](Server &server, id_t id, address_t address, json &request) -> bool {
        std::string type = request[parts::TYPE];
        int number = request[parts::NUMBER];
        int value = request[parts::VALUE];
        Server::bet_t bet{type, number, value};
        server.do_bet(id, bet);
        return false;
    };
    impl_t command_bets = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_bets(id);
        return false;
    };
    impl_t command_balance = [](Server &server, id_t id, address_t address, json &request) -> bool {
        server.do_balance(id);
        return false;
    };
    impl_t command_kick = [](Server &server, id_t id, address_t address, json &request) -> bool {
        std::string login = request[parts::LOGIN];
        server.do_kick(id, login);
        return false;
    };

    add_handler(permissions::GUEST, commands::SIGNIN, handler_signin);
    add_handler(permissions::WAIT, commands::SINGOUT, handler_signout);
    add_handler(permissions::WAIT, commands::JOIN, join);
    add_handler(permissions::STAFF, commands::CREATE, create);
    add_handler(permissions::PLAY, commands::LEAVE, leave);
    add_handler(permissions::WAIT, commands::TABLES, tables);
    add_handler(permissions::PLAY, commands::USERS, users);
    add_handler(permissions::PLAY, commands::WRITE, write);
    add_handler(permissions::ALL, commands::DISCONNECT, disconnect);
    add_handler(permissions::AUTH, commands::SYNC, sync);
    add_handler(permissions::GUEST, commands::SINGUP, handler_signup);
    add_handler(permissions::ADMIN, commands::SET_PERMITION, set_permission);
    add_handler(permissions::CROUPIER, commands::SPIN, command_spin);
    add_handler(permissions::PLAYER, commands::BET, command_bet);
    add_handler(permissions::PLAY, commands::BETS, command_bets);
    add_handler(permissions::AUTH, commands::BALANCE, command_balance);
    add_handler(permissions::CROUPIER, commands::KICK, command_kick);
}

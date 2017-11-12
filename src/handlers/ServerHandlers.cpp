#include "ServerHandlers.h"
#include "../config.h"
#include "../../lib/json/src/json.hpp"

using json = nlohmann::json;

using impl_t = std::function<bool(Server &, id_t, address_t, const std::string &)>;

ServerHandlers::ServerHandlers() : Handlers() {
    impl_t login_handler = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        auto &&request = json::parse(message);
        auto &&login = request[parts::LOGIN];
        auto &&password = request[parts::PASSWORD];
        server.login(id, login, password);
        return false;
    };
    impl_t logout = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        server.logout(id);
        return false;
    };
    impl_t join = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        auto &&request = json::parse(message);
        auto &&name = request[parts::NAME];
        auto &&password = request[parts::PASSWORD];
        server.join(id, name, password);
        return false;
    };
    impl_t create = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        auto &&request = json::parse(message);
        auto &&name = request[parts::NAME];
        auto &&password = request[parts::PASSWORD];
        server.create(id, name, password);
        return false;
    };
    impl_t leave = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        server.leave(id);
        return false;
    };
    impl_t tables = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        server.tables(id);
        return false;
    };
    impl_t users = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        server.users(id);
        return false;
    };
    impl_t write = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        auto &&request = json::parse(message);
        auto &&msg = request[parts::MESSAGE];
        if (request.count(parts::LOGIN) > 0) {
            auto &&login = request[parts::LOGIN];
            server.write(id, login, msg);
        } else {
            server.write(id, msg);
        }
        return false;
    };
    impl_t disconnect = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        return true;
    };
    impl_t sync = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        server.sync(id);
        return false;
    };
    impl_t registration = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        auto &&request = json::parse(message);
        auto &&login = request[parts::LOGIN];
        auto &&password = request[parts::PASSWORD];
        server.registration(id, login, password);
        return false;
    };
    impl_t set_permition = [](Server &server, id_t id, address_t address, const std::string &message) -> bool {
        auto &&request = json::parse(message);
        auto &&login = request[parts::LOGIN];
        auto &&permition = request[parts::PERMITION];
        server.set_permition(id, login, permition);
        return false;
    };

    add_handler(permitions::GUEST, commands::LOGIN, login_handler);
    add_handler(permitions::AUTH, commands::LOGOUT, logout);
    add_handler(permitions::AUTH, commands::JOIN, join);
    add_handler(permitions::STAFF, commands::CREATE, create);
    add_handler(permitions::PARTY, commands::LEAVE, leave);
    add_handler(permitions::AUTH, commands::TABLES, tables);
    add_handler(permitions::PARTY, commands::USERS, users);
    add_handler(permitions::PARTY, commands::WRITE, write);
    add_handler(permitions::ALL, commands::DISCONNECT, disconnect);
    add_handler(permitions::ALL, commands::SYNC, sync);
    add_handler(permitions::GUEST, commands::REGISTRATION, registration);
    add_handler(permitions::ADMIN, commands::SET_PERMITION, set_permition);
}

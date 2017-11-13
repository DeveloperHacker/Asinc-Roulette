#include "ClientHandlers.h"
#include "../config.h"
#include "../Client.h"
#include "../../lib/json/src/json.hpp"

using json = nlohmann::json;
using handler_t = std::function<void(Client &, json &)>;

ClientHandlers::ClientHandlers() : Handlers() {
    handler_t login = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t logout = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t join = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t create = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t leave = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t tables = [](Client &client, json &response) {
        auto &&list = response[parts::LIST];
        std::cout << " " << std::setw(10) << std::left << "name"
                  << std::setw(10) << std::left << "players"
                  << std::setw(10) << std::left << "max" << "lock"
                  << std::endl;
        for (auto &&entry : list) {
            std::string name = entry[parts::NAME];
            int num_players = entry[parts::NUM_PLAYERS];
            int max_players = entry[parts::MAX_PLAYERS];
            bool is_lock = entry[parts::LOCK];
            std::cout << " " << std::setw(10) << std::left << name
                      << std::setw(10) << std::left << num_players
                      << std::setw(10) << std::left << max_players
                      << is_lock << std::endl;
        }
    };
    handler_t users = [](Client &client, json &response) {
        auto &&list = response[parts::LIST];
        std::cout << " " << std::setw(10) << std::left << "login" << "role" << std::endl;
        for (auto &&entry : list) {
            std::string login = entry[parts::LOGIN];
            std::string role = entry[parts::ROLE];
            std::cout << " " << std::setw(10) << std::left << login << role << std::endl;
        }
    };
    handler_t write = [](Client &client, json &response) {
        std::string name = response[parts::NAME];
        std::string message = response[parts::MESSAGE];
        std::cout << " " << std::setw(10) << std::left << name << "::" << message << std::endl;
    };
    handler_t sync = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t registration = [](Client &client, json &response) {};
    handler_t set_permition = [](Client &client, json &response) {};

    add_handler(permitions::ALL, commands::LOGIN, login);
    add_handler(permitions::ALL, commands::LOGOUT, logout);
    add_handler(permitions::ALL, commands::JOIN, join);
    add_handler(permitions::ALL, commands::CREATE, create);
    add_handler(permitions::ALL, commands::LEAVE, leave);
    add_handler(permitions::ALL, commands::TABLES, tables);
    add_handler(permitions::ALL, commands::USERS, users);
    add_handler(permitions::ALL, commands::WRITE, write);
    add_handler(permitions::ALL, commands::SYNC, sync);
    add_handler(permitions::ALL, commands::REGISTRATION, registration);
    add_handler(permitions::ALL, commands::SET_PERMITION, set_permition);
}

void
ClientHandlers::execute(permition_t permition, const std::string &name, Client &client, json &message) {
    Handlers<void, Client &, json &>::execute(permition, name, client, message);
    std::cout << name << " success" << std::endl;
}

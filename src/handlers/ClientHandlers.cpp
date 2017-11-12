#include "ClientHandlers.h"
#include "../config.h"
#include "../Client.h"
#include "../../lib/json/src/json.hpp"

using json = nlohmann::json;
using handler_t = std::function<void(Client &, const std::string &)>;

ClientHandlers::ClientHandlers() : Handlers() {
    handler_t login = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t logout = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t join = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t create = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t leave = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t tables = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&list = response[parts::LIST];
        std::cout << " " << std::setw(10) << std::left << "name"
                  << std::setw(10) << std::left << "players"
                  << std::setw(10) << std::left << "max" << "lock"
                  << std::endl;
        for (auto &&entry : list) {
            auto &&name = entry[parts::NAME];
            auto &&num_players = entry[parts::NUM_PLAYERS];
            auto &&max_players = entry[parts::MAX_PLAYERS];
            auto &&is_lock = entry[parts::LOCK];
            std::cout << " " << std::setw(10) << std::left << name
                      << std::setw(10) << std::left << num_players
                      << std::setw(10) << std::left << max_players
                      << is_lock << std::endl;
        }
    };
    handler_t users = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&list = response[parts::LIST];
        std::cout << " " << std::setw(10) << std::left << "name" << "role" << std::endl;
        for (auto &&entry : list) {
            auto &&name = entry[parts::LOGIN];
            auto &&role = entry[parts::ROLE];
            std::cout << " " << std::setw(10) << std::left << name << role << std::endl;
        }
    };
    handler_t write = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&name = response[parts::NAME];
        auto &&msg = response[parts::MESSAGE];
        std::cout << " " << std::setw(10) << std::left << name << "::" << msg << std::endl;
    };
    handler_t sync = [](Client &client, const std::string &message) {
        auto &&response = json::parse(message);
        auto &&permition = response[parts::PERMITION];
        client.permition = permition;
    };
    handler_t registration = [](Client &client, const std::string &message) {};
    handler_t set_permition = [](Client &client, const std::string &message) {};

    add_handler(permitions::ALL, commands::LOGIN, login);
    add_handler(permitions::ALL, commands::LOGOUT, logout);
    add_handler(permitions::ALL, commands::JOIN, join);
    add_handler(permitions::ALL, commands::CREATE, create);
    add_handler(permitions::ALL, commands::LEAVE, leave);
    add_handler(permitions::ALL, commands::TABLES, tables);
    add_handler(permitions::ALL, commands::USERS, users);
    add_handler(permitions::ALL, commands::WRITE, write);
    add_handler(permitions::ALL, commands::WRITE, sync);
    add_handler(permitions::ALL, commands::WRITE, registration);
    add_handler(permitions::ALL, commands::WRITE, set_permition);
}

void
ClientHandlers::execute(permition_t permition, const std::string &name, Client &client, const std::string &message) {
    Handlers<void, Client &, const std::string &>::execute(permition, name, client, message);
    std::cout << name << " success" << std::endl;
}

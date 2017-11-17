#include "ClientHandlers.h"
#include "../config.h"
#include "../Client.h"
#include "../../lib/json/src/json.hpp"

using json = nlohmann::json;
using handler_t = std::function<void(Client &, json &)>;

ClientHandlers::ClientHandlers() : Handlers() {
    handler_t signup = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
        std::cout << "signup success" << std::endl;
    };
    handler_t signout = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
        std::cout << "signout success" << std::endl;
    };
    handler_t join = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
        std::cout << "join success" << std::endl;
    };
    handler_t create = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
        std::cout << "create success" << std::endl;
    };
    handler_t leave = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
        std::cout << "leave success" << std::endl;
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
                      << " " << std::setw(10) << std::left << num_players
                      << " " << std::setw(10) << std::left << max_players
                      << " " << is_lock << std::endl;
        }
    };
    handler_t users = [](Client &client, json &response) {
        auto &&list = response[parts::LIST];
        std::cout << " " << std::setw(10) << std::left << "login"
                  << " " << std::setw(10) << std::left << "role" << std::endl;
        for (auto &&entry : list) {
            std::string login = entry[parts::LOGIN];
            std::string role = entry[parts::ROLE];
            std::cout << " " << std::setw(10) << std::left << login
                      << " " << std::setw(10) << std::left << role << std::endl;
        }
    };
    handler_t write = [](Client &client, json &response) {
        std::string login = response[parts::LOGIN];
        std::string message = response[parts::MESSAGE];
        std::cout << " " << std::setw(10) << std::left << login
                  << " " << std::setw(10) << std::left << message << std::endl;
    };
    handler_t sync = [](Client &client, json &response) {
        permition_t permition = response[parts::PERMITION];
        client.permition = permition;
        std::cout << "sync success" << std::endl;
    };
    handler_t signin = [](Client &client, json &response) {
        std::cout << "registration success" << std::endl;
    };
    handler_t set_permition = [](Client &client, json &response) {
        std::cout << "permition set success" << std::endl;
    };
    handler_t command_spin = [](Client &client, json &response) {
        std::cout << "permition set success" << std::endl;
    };
    handler_t command_bet = [](Client &client, json &response) {
        std::cout << "bet success" << std::endl;
    };
    handler_t command_bets = [](Client &client, json &response) {
        std::cout << " " << std::setw(10) << std::left << "login"
                  << " " << std::setw(10) << std::left << "type"
                  << " " << std::setw(10) << std::left << "number"
                  << " " << std::setw(10) << std::left << "value"
                  << std::endl;
        for (auto &&it = response.begin(); it != response.end(); ++it) {
            std::string login = it.key();
            json bets = it.value();
            for (auto &&bet : bets) {
                std::string type = response[parts::BET_TYPE];
                int number = response[parts::BET_NUMBER];
                int value = response[parts::BET_VALUE];
                std::cout << " " << std::setw(10) << std::left << login
                          << " " << std::setw(10) << std::left << type
                          << " " << std::setw(10) << std::left << number
                          << " " << std::setw(10) << std::left << value
                          << std::endl;
            }
        }
    };
    handler_t command_balance = [](Client &client, json &response) {
        int balance = response[parts::BALANCE];
        std::cout << " " << balance << "$" << std::endl;
    };

    add_handler(permitions::ALL, commands::SIGNUP, signup);
    add_handler(permitions::ALL, commands::SINGOUT, signout);
    add_handler(permitions::ALL, commands::JOIN, join);
    add_handler(permitions::ALL, commands::CREATE, create);
    add_handler(permitions::ALL, commands::LEAVE, leave);
    add_handler(permitions::ALL, commands::TABLES, tables);
    add_handler(permitions::ALL, commands::USERS, users);
    add_handler(permitions::ALL, commands::WRITE, write);
    add_handler(permitions::ALL, commands::SYNC, sync);
    add_handler(permitions::ALL, commands::SIGNIN, signin);
    add_handler(permitions::ALL, commands::SET_PERMITION, set_permition);
    add_handler(permitions::ALL, commands::SPIN, command_spin);
    add_handler(permitions::ALL, commands::BET, command_bet);
    add_handler(permitions::ALL, commands::BETS, command_bets);
    add_handler(permitions::ALL, commands::BALANCE, command_balance);
}

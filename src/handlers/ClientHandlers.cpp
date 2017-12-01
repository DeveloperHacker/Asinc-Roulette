#include "ClientHandlers.h"
#include "../core/config.h"
#include "../core/Client.h"

using json = nlohmann::json;
using handler_t = std::function<void(Client &, json &)>;

ClientHandlers::ClientHandlers() : Handlers() {
    handler_t signin = [](Client &client, json &response) {
        permission_t permission = response[parts::PERMITION];
        client.permission = permission;
        std::cout << "signin success" << std::endl;
    };
    handler_t signout = [](Client &client, json &response) {
        permission_t permission = response[parts::PERMITION];
        client.permission = permission;
        std::cout << "signout success" << std::endl;
    };
    handler_t join = [](Client &client, json &response) {
        permission_t permission = response[parts::PERMITION];
        client.permission = permission;
        std::cout << "join success" << std::endl;
    };
    handler_t create = [](Client &client, json &response) {
        permission_t permission = response[parts::PERMITION];
        client.permission = permission;
        std::cout << "create success" << std::endl;
    };
    handler_t leave = [](Client &client, json &response) {
        permission_t permission = response[parts::PERMITION];
        client.permission = permission;
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
        permission_t permission = response[parts::PERMITION];
        client.permission = permission;
        std::cout << "sync success" << std::endl;
    };
    handler_t signup = [](Client &client, json &response) {
        std::cout << "registration success" << std::endl;
    };
    handler_t set_permission = [](Client &client, json &response) {
        std::cout << "permission set success" << std::endl;
    };
    handler_t command_spin = [](Client &client, json &response) {
        int random_number = response[parts::RANDOM_NUMBER];
        std::cout << "dropped the number " << random_number << std::endl;
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
        for (auto &&bet : response) {
            std::string login = bet[parts::LOGIN];
            std::string type = bet[parts::TYPE];
            int number = bet[parts::NUMBER];
            int value = bet[parts::VALUE];
            std::cout << " " << std::setw(10) << std::left << login
                      << " " << std::setw(10) << std::left << type
                      << " " << std::setw(10) << std::left << number
                      << " " << std::setw(10) << std::left << value
                      << std::endl;
        }
    };
    handler_t command_balance = [](Client &client, json &response) {
        int balance = response[parts::BALANCE];
        std::cout << " " << balance << "$" << std::endl;
    };
    handler_t command_pay = [](Client &client, json &response) {
        std::string type = response[parts::TYPE];
        int number = response[parts::NUMBER];
        int value = response[parts::VALUE];
        std::cout << "the bet " << type << " " << number << " played " << value << "$" << std::endl;
    };
    handler_t command_kick = [](Client &client, json &response) {
        std::cout << "user kicked" << std::endl;
    };

    add_handler(permissions::ALL, commands::SIGNIN, signin);
    add_handler(permissions::ALL, commands::SINGOUT, signout);
    add_handler(permissions::ALL, commands::JOIN, join);
    add_handler(permissions::ALL, commands::CREATE, create);
    add_handler(permissions::ALL, commands::LEAVE, leave);
    add_handler(permissions::ALL, commands::TABLES, tables);
    add_handler(permissions::ALL, commands::USERS, users);
    add_handler(permissions::ALL, commands::WRITE, write);
    add_handler(permissions::ALL, commands::SYNC, sync);
    add_handler(permissions::ALL, commands::SINGUP, signup);
    add_handler(permissions::ALL, commands::SET_PERMITION, set_permission);
    add_handler(permissions::ALL, commands::SPIN, command_spin);
    add_handler(permissions::ALL, commands::BET, command_bet);
    add_handler(permissions::ALL, commands::BETS, command_bets);
    add_handler(permissions::ALL, commands::BALANCE, command_balance);
    add_handler(permissions::ALL, commands::PAY, command_pay);
    add_handler(permissions::ALL, commands::KICK, command_kick);
}

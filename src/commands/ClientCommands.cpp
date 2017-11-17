#include "ClientCommands.h"
#include "../config.h"

ClientCommands::ClientCommands() : Commands() {}

void ClientCommands::init(Client &client) {
    impl_t help = [this](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected help arguments");
        for (auto &&entry: get_commands_info(permition)) {
            auto &&name = entry.first;
            auto &&argument_description = std::get<0>(entry.second);
            auto &&description = std::get<1>(entry.second);
            auto &&signature = name + " " + argument_description;
            std::cout << " " << std::setw(30) << std::left << signature
                      << std::left << description << std::endl;
        }
    };
    impl_t signup = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() != 2)
            throw ClientCommands::error("expected only login and password");
        client.login(arguments[0], arguments[1]);
    };
    impl_t singout = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected logout arguments");
        client.logout();
    };
    impl_t join = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() == 2) {
            client.join(arguments[0], arguments[1]);
            return;
        }
        if (arguments.size() == 1) {
            client.join(arguments[0], "");
            return;
        }
        throw ClientCommands::error("expected name table and optional password");
    };
    impl_t create = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() == 2) {
            client.create(arguments[0], arguments[1]);
            return;
        }
        if (arguments.size() == 1) {
            client.create(arguments[0], "");
            return;
        }
        throw ClientCommands::error("expected name table and optional password");
    };
    impl_t leave = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected leave arguments");
        client.leave();
    };
    impl_t tables = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected list arguments");
        client.tables();
    };
    impl_t users = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected users arguments");
        client.users();
    };
    impl_t write = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() == 1)
            client.write(arguments[0]);
        else if (arguments.size() == 2)
            client.write(arguments[0], arguments[1]);
        else
            throw ClientCommands::error("expected optiponal login and message arguments");
    };
    impl_t disconnect = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected disconnect arguments");
        client.disconnect();
    };
    impl_t sync = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected sync arguments");
        client.sync();
    };
    impl_t signin = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() != 2)
            throw ClientCommands::error("expected only login and password");
        client.registration(arguments[0], arguments[1]);
    };
    impl_t set_permition = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() != 2)
            throw ClientCommands::error("expected only login and new permition");
        int new_permition;
        if (!strings::str2int(new_permition, arguments[1]) || new_permition < 0)
            throw ClientCommands::error("permition parse error");
        client.set_permition(arguments[0], static_cast<permition_t>(new_permition));
    };
    impl_t command_spin = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected spin arguments");
        client.spin();
    };
    impl_t command_bet = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() != 3)
            throw ClientCommands::error("expected only type, number and value");
        int number, value;
        if (!strings::str2int(number, arguments[1]) || number < 0)
            throw ClientCommands::error("bet number parse error");
        if (!strings::str2int(value, arguments[2]) || value < 0)
            throw ClientCommands::error("bet value parse error");
        client.bet(arguments[0], number, value);
    };
    impl_t command_bets = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected bets arguments");
        client.bets();
    };
    impl_t command_balance = [this, &client](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected balance arguments");
        client.balance();
    };

    std::string help_description("show help");
    std::string login_description("sign up at the system");
    std::string logout_description("sign out from the system");
    std::string join_description("join to the table");
    std::string create_description("create the table");
    std::string leave_description("leave from the table");
    std::string tables_description("show a list of the tables");
    std::string users_description("show a list of users at the table");
    std::string write_description("write a message into users at the table");
    std::string disconnect_description("disconnect from the server");
    std::string sync_description("synchronize permitions in the system");
    std::string registration_description("registration in the system");
    std::string set_permition_desc("set permitions from user in the system");
    std::string spin_description("spin roulette");
    std::string bet_description("make bet");
    std::string bets_description("get bets of all users");
    std::string balance_description("get balance");

    add_command(permitions::ALL, commands::HELP, "", help_description, help);
    add_command(permitions::GUEST, commands::SIGNUP, "login password", login_description, signup);
    add_command(permitions::WAIT, commands::SINGOUT, "", logout_description, singout);
    add_command(permitions::WAIT, commands::JOIN, "name [password]", join_description, join);
    add_command(permitions::STAFF, commands::CREATE, "name [password]", create_description, create);
    add_command(permitions::PLAY, commands::LEAVE, "", leave_description, leave);
    add_command(permitions::WAIT, commands::TABLES, "", tables_description, tables);
    add_command(permitions::PLAY, commands::USERS, "", users_description, users);
    add_command(permitions::PLAY, commands::WRITE, "[login] message", write_description, write);
    add_command(permitions::ALL, commands::DISCONNECT, "", disconnect_description, disconnect);
    add_command(permitions::AUTH, commands::SYNC, "", sync_description, sync);
    add_command(permitions::GUEST, commands::SIGNIN, "login password", registration_description, signin);
    add_command(permitions::ADMIN, commands::SET_PERMITION, "login permition", set_permition_desc, set_permition);
    add_command(permitions::CROUPIER, commands::SPIN, "", spin_description, command_spin);
    add_command(permitions::PLAYER, commands::BET, "type number value", bet_description, command_bet);
    add_command(permitions::PLAY, commands::BETS, "", bets_description, command_bets);
    add_command(permitions::AUTH, commands::BALANCE, "", balance_description, command_balance);
}

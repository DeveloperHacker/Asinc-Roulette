#include "ClientCommands.h"
#include "../core/config.h"

ClientCommands::ClientCommands() : Commands() {}

void ClientCommands::init(Client &client) {
    impl_t help = [this](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected help arguments");
        for (auto &&entry: get_commands_info(permission)) {
            auto &&name = entry.first;
            auto &&argument_description = std::get<0>(entry.second);
            auto &&description = std::get<1>(entry.second);
            auto &&signature = name + " " + argument_description; // NOLINT
            std::cout << " " << std::setw(30) << std::left << signature
                      << std::left << description << std::endl;
        }
    };
    impl_t signin = [this, &client](permission_t permission, const args_t &arguments) {
        if (arguments.size() != 2)
            throw ClientCommands::error("expected only login and password");
        client.login(arguments[0], arguments[1]);
    };
    impl_t singout = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected logout arguments");
        client.logout();
    };
    impl_t join = [this, &client](permission_t permission, const args_t &arguments) {
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
    impl_t create = [this, &client](permission_t permission, const args_t &arguments) {
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
    impl_t leave = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected leave arguments");
        client.leave();
    };
    impl_t tables = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected list arguments");
        client.tables();
    };
    impl_t users = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected users arguments");
        client.users();
    };
    impl_t write = [this, &client](permission_t permission, const args_t &arguments) {
        if (arguments.size() == 1)
            client.write(arguments[0]);
        else if (arguments.size() == 2)
            client.write(arguments[0], arguments[1]);
        else
            throw ClientCommands::error("expected optiponal login and message arguments");
    };
    impl_t disconnect = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected disconnect arguments");
        client.disconnect();
    };
    impl_t sync = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected sync arguments");
        client.sync();
    };
    impl_t signup = [this, &client](permission_t permission, const args_t &arguments) {
        if (arguments.size() != 2)
            throw ClientCommands::error("expected only login and password");
        client.registration(arguments[0], arguments[1]);
    };
    impl_t set_permission = [this, &client](permission_t permission, const args_t &arguments) {
        if (arguments.size() != 2)
            throw ClientCommands::error("expected only login and new permission");
        int new_permission;
        if (!strings::str2int(new_permission, arguments[1]) || new_permission < 0)
            throw ClientCommands::error("permission parse error");
        client.set_permission(arguments[0], static_cast<permission_t>(new_permission));
    };
    impl_t command_spin = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected spin arguments");
        client.spin();
    };
    impl_t command_bet = [this, &client](permission_t permission, const args_t &arguments) {
        if (arguments.size() != 3)
            throw ClientCommands::error("expected only type, number and value");
        int number, value;
        if (!strings::str2int(number, arguments[1]) || number < 0)
            throw ClientCommands::error("bet number parse error");
        if (!strings::str2int(value, arguments[2]) || value < 0)
            throw ClientCommands::error("bet value parse error");
        client.bet(arguments[0], number, value);
    };
    impl_t command_bets = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected bets arguments");
        client.bets();
    };
    impl_t command_balance = [this, &client](permission_t permission, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error("unexpected balance arguments");
        client.balance();
    };
    impl_t command_kick = [this, &client](permission_t permission, const args_t &arguments) {
        if (arguments.size() != 1)
            throw ClientCommands::error("expected only login");
        client.kick(arguments[0]);
    };

    std::string help_description("show help");
    std::string signin_description("sign in the system");
    std::string signout_description("sign out from the system");
    std::string join_description("join to the table");
    std::string create_description("create the table");
    std::string leave_description("leave from the table");
    std::string tables_description("show a list of the tables");
    std::string users_description("show a list of users at the table");
    std::string write_description("write a message into users at the table");
    std::string disconnect_description("disconnect from the server");
    std::string sync_description("synchronize permissions in the system");
    std::string signup_description("signup in the system");
    std::string set_permission_desc("set permissions from user in the system");
    std::string spin_description("spin roulette");
    std::string bet_description("make bet");
    std::string bets_description("get bets of all users");
    std::string balance_description("get balance");
    std::string kick_description("kick player");

    add_command(permissions::ALL, commands::HELP, "", help_description, help);
    add_command(permissions::GUEST, commands::SIGNIN, "login password", signin_description, signin);
    add_command(permissions::WAIT, commands::SINGOUT, "", signout_description, singout);
    add_command(permissions::WAIT, commands::JOIN, "name [password]", join_description, join);
    add_command(permissions::STAFF, commands::CREATE, "name [password]", create_description, create);
    add_command(permissions::PLAY, commands::LEAVE, "", leave_description, leave);
    add_command(permissions::WAIT, commands::TABLES, "", tables_description, tables);
    add_command(permissions::PLAY, commands::USERS, "", users_description, users);
    add_command(permissions::PLAY, commands::WRITE, "[login] message", write_description, write);
    add_command(permissions::ALL, commands::DISCONNECT, "", disconnect_description, disconnect);
    add_command(permissions::AUTH, commands::SYNC, "", sync_description, sync);
    add_command(permissions::GUEST, commands::SINGUP, "login password", signup_description, signup);
    add_command(permissions::ADMIN, commands::SET_PERMISSION, "login permission", set_permission_desc, set_permission);
    add_command(permissions::CROUPIER, commands::SPIN, "", spin_description, command_spin);
    add_command(permissions::PLAYER, commands::BET, "type number value", bet_description, command_bet);
    add_command(permissions::PLAY, commands::BETS, "", bets_description, command_bets);
    add_command(permissions::AUTH, commands::BALANCE, "", balance_description, command_balance);
    add_command(permissions::CROUPIER, commands::KICK, "login", kick_description, command_kick);
}

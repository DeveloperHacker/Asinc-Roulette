#include "ClientCommands.h"
#include "../config.h"

ClientCommands::ClientCommands() : Commands() {}

void ClientCommands::init(Client &client) {
    impl_t help = [this](permition_t permition, const args_t &arguments) {
        if (!arguments.empty())
            throw ClientCommands::error( "unexpected help arguments");
        for (auto &&entry: get_commands_info(permition)) {
            auto &&name = entry.first;
            auto &&description = entry.second;
            std::cout << " " << std::setw(15) << std::left << name << std::left << description << std::endl;
        }
    };
    impl_t login = [this, &client](permition_t permition, const args_t &arguments) {
        if (arguments.size() != 2)
            throw ClientCommands::error("expected only login and password");
        client.login(arguments[0], arguments[1]);
    };
    impl_t logout = [this, &client](permition_t permition, const args_t &arguments) {
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
        if (arguments.size() != 1)
            throw ClientCommands::error("expected only one message arguments");
        client.write(arguments[0]);
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
    impl_t registration = [this, &client](permition_t permition, const args_t &arguments) {
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

    std::string help_name("help");
    std::string help_description("show help");
    std::string login_description("login to system");
    std::string logout_description("logout from system");
    std::string join_description("join to table");
    std::string create_description("create table");
    std::string leave_description("leave from table");
    std::string tables_description("show list of tables");
    std::string users_description("show list of users at table");
    std::string write_description("write message into users at table");
    std::string disconnect_description("disconnect");
    std::string sync_description("synchronize permitions in system");
    std::string registration_description("registration in system");
    std::string set_permition_description("set permitions from user in system");

    add_command(permitions::ALL, help_name, help_description, help);
    add_command(permitions::GUEST, commands::LOGIN, login_description, login);
    add_command(permitions::AUTH, commands::LOGOUT, logout_description, logout);
    add_command(permitions::AUTH, commands::JOIN, join_description, join);
    add_command(permitions::STAFF, commands::CREATE, create_description, create);
    add_command(permitions::PARTY, commands::LEAVE, leave_description, leave);
    add_command(permitions::AUTH, commands::TABLES, tables_description, tables);
    add_command(permitions::PARTY, commands::USERS, users_description, users);
    add_command(permitions::PARTY, commands::WRITE, write_description, write);
    add_command(permitions::ALL, commands::DISCONNECT, disconnect_description, disconnect);
    add_command(permitions::ALL, commands::SYNC, sync_description, sync);
    add_command(permitions::GUEST, commands::REGISTRATION, registration_description, registration);
    add_command(permitions::ADMIN, commands::SET_PERMITION, set_permition_description, set_permition);
}

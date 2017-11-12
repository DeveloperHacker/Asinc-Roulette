#pragma once

#include "../commands/Commands.h"

class SimpleServerCommands : public Commands {
public:
    explicit SimpleServerCommands(TCPServer &server) : Commands() {
        impl_t help = [this](permition_t permition, const args_t &arguments) -> bool {
            if (!arguments.empty()) {
                std::cerr << "unexpected help arguments" << std::endl;
                return false;
            }
            for (auto &&entry: get_commands_info(permition)) {
                auto &&name = entry.first;
                auto &&description = entry.second;
                std::cout << " " << std::setw(10) << std::left << name << std::left << description << std::endl;
            }
            return true;
        };
        impl_t shutdown = [&server](permition_t permition, const args_t &arguments) -> bool {
            if (!arguments.empty()) {
                std::cerr << "unexpected shutdown arguments" << std::endl;
                return false;
            }
            server.stop();
            return true;
        };
        impl_t kill = [&server](permition_t permition, const args_t &arguments) -> bool {
            if (arguments.empty()) {
                std::cerr << "expected kill argument" << std::endl;
                return false;
            }
            for (auto &&argument : arguments) {
                int id;
                if (strings::str2int(id, argument) && id >= 0) {
                    server.kill(static_cast<id_t>(id));
                } else if (argument == "--all" || argument == "-a") {
                    auto &&connections = server.get_connections();
                    for (auto &&connection: connections) {
                        server.kill(connection.first);
                    }
                } else {
                    std::cerr << "kill argument " << argument << " hasn't recognized" << std::endl;
                    return false;
                }
            }
            return true;
        };
        impl_t list = [&server](permition_t permition, const args_t &arguments) -> bool {
            if (!arguments.empty()) {
                std::cerr << "unexpected list arguments" << std::endl;
                return false;
            }
            auto &&connections = server.get_connections();
            if (connections.empty()) {
                std::cout << "connections list is empty" << std::endl;
                return true;
            }
            std::cout << " "
                      << std::setw(5) << std::left << "id"
                      << std::setw(15) << std::left << "host"
                      << "port" << std::endl;
            for (auto &&connection: connections) {
                auto &&id = connection.first;
                auto &&address = connection.second;
                auto &&host = inet_ntoa(address.sin_addr);
                auto &&port = address.sin_port;
                std::cout << " "
                          << std::setw(5) << std::left << id
                          << std::setw(15) << std::left << host
                          << port << std::endl;
            }
            return true;
        };
        impl_t send = [&server](permition_t permition, const args_t &arguments) -> bool {
            if (arguments.empty()) {
                std::cerr << "expected kill argument" << std::endl;
                return false;
            }
            auto &&message = arguments[arguments.size() - 1];
            if (arguments.size() == 1) {
                server.broadcast(message);
                return true;
            }
            std::vector<id_t> ids;
            for (int i = 0; i < arguments.size() - 1; ++i) {
                auto &&argument = arguments[i];
                int id;
                if (strings::str2int(id, argument) && id >= 0) {
                    ids.push_back(static_cast<id_t>(id));
                } else {
                    std::cerr << "send id " << argument << " hasn't recognized" << std::endl;
                }
            }
            server.send(ids, message);
            return true;
        };

        std::string help_name("help");
        std::string shutdown_name("shutdown");
        std::string kill_name("kill");
        std::string list_name("list");
        std::string send_name("send");

        std::string help_description("show help");
        std::string shutdown_description("shutdown server");
        std::string kill_description("kill clients with specified ids or killing all clients -a, --all");
        std::string list_description("show list of connected clients");
        std::string send_description("send message to client with specified ids");

        add_command(permitions::ADMIN, help_name, help_description, help);
        add_command(permitions::ADMIN, shutdown_name, shutdown_description, shutdown);
        add_command(permitions::ADMIN, kill_name, kill_description, kill);
        add_command(permitions::ADMIN, list_name, list_description, list);
        add_command(permitions::ADMIN, send_name, send_description, send);
    }
};


#include "Command.h"
#include "../simple/EchoServer.h"
#include "../simple/config.h"

using CommandArgs = std::vector<std::string>;
using CommndExpr = std::function<bool(const CommandArgs &)>;
using Commands = std::unordered_map<std::string, std::pair<std::string, CommndExpr>>;

bool str2int(int &integer, const std::string &string) {
    try {
        size_t position;
        integer = stoi(string, &position);
        return position == string.size();
    }
    catch (const std::invalid_argument &) {
        return false;
    }
}

void init(TCPServer &server, Commands &commands) {
    auto &&help = [&commands](const CommandArgs &arguments) -> bool {
        if (!arguments.empty()) {
            std::cerr << "unexpected shutdown arguments" << std::endl;
        }
        for (auto &&entry: commands) {
            auto &&name = entry.first;
            auto &&command = entry.second;
            auto &&description = command.first;
            std::cout << name << " -- " << description << std::endl;
        }
        return true;
    };
    auto &&shutdown = [&server](const CommandArgs &arguments) -> bool {
        if (!arguments.empty()) {
            std::cerr << "unexpected shutdown arguments" << std::endl;
            return false;
        }
        server.stop();
        return true;
    };
    auto &&kill = [&server](const CommandArgs &arguments) -> bool {
        if (arguments.empty()) {
            std::cerr << "expected kill argument" << std::endl;
            return false;
        }
        for (auto &&argument : arguments) {
            int id;
            if (str2int(id, argument) && id >= 0) {
                server.kill(static_cast<id_t>(id));
            } else if (argument == "--all" || argument == "-a") {
                auto &&connections = server.get_connections();
                for (auto &&connection: connections) {
                    server.kill(connection.first);
                }
            } else {
                std::cerr << "kill argument " << argument << " hasn't recognized" << std::endl;
            }
        }
        return true;
    };
    auto &&list = [&server](const CommandArgs &arguments) -> bool {
        if (!arguments.empty()) {
            std::cerr << "unexpected list arguments" << std::endl;
            return false;
        }
        auto &&connections = server.get_connections();
        if (connections.empty()) {
            std::cout << "connections list is empty" << std::endl;
            return true;
        }
        std::cout << std::setw(4) << "id" << std::setw(15) << "host" << std::setw(10) << "port" << std::endl;
        for (auto &&connection: connections) {
            auto &&id = connection.first;
            auto &&address = connection.second;
            auto &&host = inet_ntoa(address.sin_addr);
            auto &&port = address.sin_port;
            std::cout << std::setw(4) << id << std::setw(15) << host << std::setw(10) << port << std::endl;
        }
        return true;
    };
    auto &&send = [&server](const CommandArgs &arguments) -> bool {
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
            if (str2int(id, argument) && id >= 0) {
                ids.push_back(static_cast<id_t>(id));
            } else {
                std::cerr << "send id " << argument << " hasn't recognized" << std::endl;
            }
        }
        server.send(ids, message);
        return true;
    };
    commands.emplace("help", std::make_pair("show help", help));
    commands.emplace("shutdown", std::make_pair("shutdown server", shutdown));
    auto &&kill_description = "kill clients, arguments: list of client ids for killing or -a, --all for killing all clients";
    commands.emplace("kill", std::make_pair(kill_description, kill));
    commands.emplace("list", std::make_pair("show list of connected clients", list));
    commands.emplace("send", std::make_pair("send message to client with specified ids", send));
}

int main() {
    auto &&address = Socket::address(address::SERVER_PORT);
    EchoServer server(AF_INET, SOCK_STREAM, 0, address);
    Commands commands;
    init(server, commands);
    server.start();
    while (!server.stopped()) {
        std::string raw_command;
        std::getline(std::cin, raw_command);
        Command command(raw_command);
        auto name = command.get_name();
        auto &&inst = commands.find(name);
        if (inst != std::end(commands)) {
            auto &&foo = inst->second.second;
            auto &&success = foo(command.get_arguments());
            if (!success) {
                std::cerr << "use 'help' command for getting more information" << std::endl;
            }
        } else if (!name.empty()) {
            std::cerr << "command " << name << " hasn't recognized" << std::endl;
        }
    }
    server.join();
}
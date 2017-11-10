
#include "Command.h"
#include "simple/EchoServer.h"
#include "global.h"
#include <functional>
#include <iomanip>

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
                if (server.kill(static_cast<id_t>(id)) == 1) {
                    std::cerr << "connection " << id << " hasn't found" << std::endl;
                } else {
                    std::cout << "connection " << id << " closed" << std::endl;
                }
            } else if (argument == "--all" || argument == "-a") {
                auto &&connections = server.get_connections();
                for (auto &&connection: connections) {
                    server.kill(connection.first);
                }
                std::cout << "all connection closed" << std::endl;
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
        }
        for (auto &&connection: connections) {
            auto &&id = connection.first;
            auto &&address = connection.second;
            std::cout << std::setw(4) << id << std::setw(10) << address << std::endl;
        }
        return true;
    };
    commands.emplace("help", std::make_pair("show help", help));
    commands.emplace("shutdown", std::make_pair("shutdown server", shutdown));
    auto &&kill_description = "kill clients, arguments: list of client ids for killing or -a, --all for killing all clients";
    commands.emplace("kill", std::make_pair(kill_description, kill));
    commands.emplace("list", std::make_pair("show list of connected clients", list));
}

int main() {
    auto &&address = Socket::address(global::address::SERVER_PORT);
    EchoServer server(AF_INET, SOCK_STREAM, 0, address);
    Commands commands;
    init(server, commands);
    server.start();
    while (!server.stopped()) {
        std::string raw_command;
        std::getline(std::cin, raw_command);
        Command command(raw_command);
        auto name = command.get_name();
        if (commands.count(name) == 1) {
            auto &&foo = commands.find(name)->second.second;
            auto &&success = foo(command.get_arguments());
            if (!success) {
                std::cerr << "use 'help' command for getting more information" << std::endl;
            }
        } else {
            std::cerr << "command " << name << " hasn't recognized" << std::endl;
        }
    }
    server.join();
}
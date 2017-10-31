
#include "Command.h"
#include "simple/EchoServer.h"
#include "Config.h"
#include <arpa/inet.h>
#include <functional>

using CommandArgs = std::vector<std::string>;
using CommndExpr = std::function<bool(const CommandArgs &)>;
using Commands = std::unordered_map<std::string, std::pair<std::string, CommndExpr>>;


void init(TCPServer &server, Commands &commands) {
    auto &&help = [&commands](const CommandArgs &arguments) -> bool {
        if (!arguments.empty()) {
            std::cerr << "Unexpected shutdown arguments" << std::endl;
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
            std::cerr << "Unexpected shutdown arguments" << std::endl;
            return false;
        }
        server.stop();
        return true;
    };
    auto &&kill = [&server](const CommandArgs &arguments) -> bool {
        if (arguments.empty()) {
            std::cerr << "Expected kill argument" << std::endl;
            return false;
        }
        for (auto &&argument : arguments) {
            std::istringstream stream(argument);
            socket_t descriptor = 0;
            stream >> descriptor;
            if (!stream.fail()) {
                server.kill(descriptor);
            } else if (argument == "--all") {
                auto &&descriptors = server.get_descriptors();
                for (auto &&entry: descriptors) server.kill(entry.first);
                break;
            } else {
                std::cerr << "argument " << argument << " hasn't recognized" << std::endl;
            }
        }
        return true;
    };
    auto &&list = [&server](const CommandArgs &arguments) -> bool {
        if (!arguments.empty()) {
            std::cerr << "Unexpected list arguments" << std::endl;
            return false;
        }
        auto &&descriptors = server.get_descriptors();
        if (descriptors.empty()) {
            std::cout << "Connections hasn't found" << std::endl;
        }
        for (auto &&entry: descriptors) {
            auto &&descriptor = entry.first;
            auto &&client_address = entry.second;
            auto &&host = inet_ntoa(client_address.sin_addr);
            auto &&port = client_address.sin_port;
            printf("%-4d %10s:%d\n", descriptor, host, port);
        }
        return true;
    };
    commands.emplace("help", std::make_pair("show help", help));
    commands.emplace("shutdown", std::make_pair("shutdown server", shutdown));
    auto &&kill_description = "kill clients, arguments: list of client ids for killing or --all for killing all clients";
    commands.emplace("kill", std::make_pair(kill_description, kill));
    commands.emplace("list", std::make_pair("show list of connected clients", list));
}

int main() {
    auto &&address = Socket::address(config::address::SERVER_PORT);
    EchoServer server(AF_INET, SOCK_STREAM, 0, address);
    Commands commands;
    init(server, commands);
    while (!server.stopped()) {
        std::string raw_command;
        std::getline(std::cin, raw_command);
        Command command(raw_command);
        auto name = command.get_name();
        if (commands.count(name) == 1) {
            auto &&foo = commands.find(name)->second.second;
            auto &&success = foo(command.get_arguments());
            if (!success) {
                std::cerr << "Use 'help' command for getting more information" << std::endl;
            }
        } else {
            std::cerr << "command " << name << " hasn't recognized" << std::endl;
        }
    }
    server.join();
}

#include "Command.h"
#include "Server.h"
#include <arpa/inet.h>


int main() {
    auto &&address = Server::address();
    Server server(AF_INET, SOCK_STREAM, 0, address);
    while (!server.is_stopped()) {
        std::string raw_command;
        std::getline(std::cin, raw_command);
        Command command(raw_command);
        auto name = command.get_name();
        if (name == "help") {
            std::cout << "help             : show help" << std::endl;
            std::cout << "shutdown         : shutdown server" << std::endl;
            std::cout << "kill [(int)id,] --all : kill clients" << std::endl;
            std::cout << "list             : show list of connected clients" << std::endl;
        } else if (name == "shutdown") {
            server.stop();
        } else if (name == "kill") {
            if (command.get_arguments().empty()) {
                std::cerr << "expected kill argument" << std::endl;
                continue;
            }
            for (auto &&argument : command.get_arguments()) {
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
        } else if (name == "list") {
            auto &&descriptors = server.get_descriptors();
            for (auto &&entry: descriptors) {
                auto &&descriptor = entry.first;
                auto &&client_address = entry.second;
                auto &&host = inet_ntoa(client_address.sin_addr);
                auto &&port = client_address.sin_port;
                printf("%-4d %10s:%d\n", descriptor, host, port);
            }
        } else {
            std::cerr << "command " << name << " hasn't recognized" << std::endl;
        }
    }
    server.join();
}
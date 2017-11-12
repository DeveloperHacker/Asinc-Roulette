
#include "../commands/Command.h"
#include "Server.h"
#include "SimpleServerCommands.h"
#include "config.h"

int main() {
    auto &&address = Socket::address(address::SERVER_PORT);
    Server server(AF_INET, SOCK_STREAM, 0, address);
    SimpleServerCommands commands(server);
    server.start();
    while (!server.stopped()) {
        std::string command;
        std::getline(std::cin, command);
        commands.parse_and_execute(permitions::ADMIN, command);
    }
    server.join();
}
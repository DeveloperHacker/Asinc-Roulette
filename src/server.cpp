
#include "../commands/Command.h"
#include "config.h"
#include "../tcp/Socket.h"
#include "Server.h"
#include "commands/ServerCommands.h"

int main() {
    auto &&address = Socket::address(address::SERVER_PORT);
    Server server(AF_INET, SOCK_STREAM, 0, address);
    ServerCommands commands(server);
    server.start();
    while (!server.stopped()) {
        try {
            std::string command;
            std::getline(std::cin, command);
            commands.parse_and_execute(permitions::ADMIN, command);
        } catch (ServerCommands::error &ex) {
            std::cerr << ex.what() << std::endl;
        } catch (Server::error &ex) {
            std::cerr << ex.what() << std::endl;
        }
    }
    server.join();
}
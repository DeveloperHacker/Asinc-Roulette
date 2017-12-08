
#include "commands/Command.h"
#include "config.h"
#include "transfer/Socket.h"
#include "core/Server.h"
#include "commands/ServerCommands.h"
#include "transfer/LinuxTCPSocket.h"

int main() {
    auto &&address = LinuxTCPSocket::make_address(address::SERVER_PORT);
    auto &&socket = std::make_shared<LinuxTCPSocket>(AF_INET, SOCK_STREAM, 0);
    socket->bind(address);
    socket->set_options(SO_REUSEADDR);
    socket->listen(1);
    Server server(socket);
    ServerCommands commands(server);
    server.start();
    while (!server.stopped()) {
        try {
            std::string command;
            std::getline(std::cin, command);
            commands.parse_and_execute(permissions::ADMIN, command);
        } catch (ServerCommands::error &ex) {
            std::cerr << ex.what() << std::endl;
        } catch (Server::error &ex) {
            std::cerr << ex.what() << std::endl;
        }
    }
    server.join();
}
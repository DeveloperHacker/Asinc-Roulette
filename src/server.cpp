
#include "commands/Command.h"
#include "Config.h"
#include "transfer/Socket.h"
#include "core/Server.h"
#include "commands/ServerCommands.h"

int main() {
#ifdef _WIN32
    Socket::startup();
#endif
    std::cout << "Server allowed from port " << Config::get_port() << std::endl;
    auto &&address = Socket::make_address(Config::get_port());
    auto &&socket = std::make_shared<Socket>();
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
#ifdef _WIN32
    Socket::cleanup();
#endif
}
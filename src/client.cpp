#include "core/Client.h"
#include "Config.h"

int main() {
#ifdef _WIN32
    Socket::startup();
#endif
    std::cout << "Client connected to address " << Config::get_host() << ":" << Config::get_port() << std::endl;
    auto &&address = Socket::make_address(Config::get_host(), Config::get_port());
    auto &&socket = std::make_shared<Socket>();
    socket->connect(address);
    Client client(socket);
    client.start();
    client.join();
#ifdef _WIN32
    Socket::cleanup();
#endif
}
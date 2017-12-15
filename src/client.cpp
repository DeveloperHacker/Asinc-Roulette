#include "config.h"
#include "core/Client.h"

int main() {
#ifdef _WIN32
    Socket::startup();
#endif
    auto &&address = Socket::make_address(address::SERVER_HOST, address::SERVER_PORT);
    auto &&socket = std::make_shared<Socket>();
    socket->connect(address);
    Client client(socket);
    client.start();
    client.join();
#ifdef _WIN32
    Socket::cleanup();
#endif
}
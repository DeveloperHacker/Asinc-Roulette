#include "tcp/Socket.h"
#include "core/config.h"
#include "core/Client.h"

int main() {
#ifdef _WIN32
    auto &&address = Socket::address(AF_INET, SOCK_STREAM, 0, address::SERVER_HOST, address::SERVER_PORT);
#else
    auto &&address = Socket::address(address::SERVER_HOST, address::SERVER_PORT);
#endif
    Client client(AF_INET, SOCK_STREAM, 0, address);
    client.start();
    client.join();
}
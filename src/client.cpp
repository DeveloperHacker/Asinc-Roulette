#include "tcp/Socket.h"
#include "core/config.h"
#include "core/Client.h"


int main() {
    auto &&address = Socket::address(address::SERVER_HOST, address::SERVER_PORT);
    Client client(AF_INET, SOCK_STREAM, 0, address);
    client.start();
    client.join();
}
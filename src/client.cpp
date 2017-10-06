#include <iostream>
#include <arpa/inet.h>
#include "../tcp/Socket.h"
#include "game/Client.h"
#include "Config.h"
#include "simple/ReadClient.h"


int main() {
    auto &&address = Socket::address(config::address::SERVER_HOST, config::address::SERVER_PORT);
    ReadClient client(AF_INET, SOCK_STREAM, 0, address);
    client.join();
}
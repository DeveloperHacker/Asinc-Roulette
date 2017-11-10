#include <iostream>
#include <arpa/inet.h>
#include "../tcp/Socket.h"
#include "game/Client.h"
#include "global.h"
#include "simple/ReadClient.h"


int main() {
    auto &&address = Socket::address(global::address::SERVER_HOST, global::address::SERVER_PORT);
    ReadClient client(AF_INET, SOCK_STREAM, 0, address);
    client.start();
    client.join();
}
#include <iostream>
#include <arpa/inet.h>
#include "../tcp/Socket.h"
#include "Server.h"


int main() {
    auto &&address = Server::address();
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    Socket socket(AF_INET, SOCK_STREAM, 0);
    socket.connect(address);
    std::string message;
    while (message != "shutdown" && message != "close") {
        std::cout << "send: ";
        std::getline(std::cin, message);
        socket.send(message, 0);
        std::string report = socket.receive(0);
        std::cout << "report: '" << report << "'" << std::endl;
    }
    socket.close();
}
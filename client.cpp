#include <iostream>
#include <arpa/inet.h>
#include "src/Socket.h"


int main() {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(7777);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    Socket sock(AF_INET, SOCK_STREAM, 0);
    sock.connect(address);
    std::string message;
    while (message != "shutdown" && message != "close") {
        std::cout << "send: ";
        std::getline(std::cin, message);
        sock.send(message, 0);
        std::string report = sock.receive(0);
        std::cout << "report: '" << report << "'" << std::endl;
    }
    sock.close();
}
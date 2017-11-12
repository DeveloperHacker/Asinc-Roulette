#include "ReadClient.h"

ReadClient::ReadClient(int domain, int type, int protocol, address_t &address
) : CryptoClient(domain, type, protocol, address) {
}

void ReadClient::crypto_output() {
    std::string message;
    std::getline(std::cin, message);
    send(message);
}

void ReadClient::crypto_input(const std::string &message) {
    std::cout << "message receive " << message << std::endl;
}

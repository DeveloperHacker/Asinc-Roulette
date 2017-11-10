
#include "ReadClient.h"

ReadClient::ReadClient(int domain, int type, int protocol, address_t &address
) : TCPClient(domain, type, protocol, address), transfer(), state(INIT) {
}

void ReadClient::output(SendSocket &socket) {
    switch (state) {
        case INIT: {
            state = WAIT;
            socket.send(transfer.public_key());
        }
        case WAIT: break;
        case READY: {
            std::string message;
            std::getline(std::cin, message);
            socket.send(transfer.encrypt(message));
        }
    }
}

void ReadClient::input(const std::string &encrypted) {
    switch (state) {
        case INIT: {
            throw std::runtime_error("Protocol Error");
        }
        case WAIT: {
            auto &&message = transfer.decrypt(encrypted);
            std::cout << "message receive '" << message << "'" << std::endl;
            if (message == global::transfer::INIT_MESSAGE) {
                state = READY;
                break;
            }
            throw std::runtime_error("Protocol Error");
        }
        case READY: {
            auto &&message = transfer.decrypt(encrypted);
            std::cout << "message receive '" << message << "'" << std::endl;
            break;
        }
    }
}

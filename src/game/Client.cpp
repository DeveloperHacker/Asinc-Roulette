#include "Client.h"

Client::Client(int domain, int type, int protocol, sockaddr_in &address) :
        TCPClient(domain, type, protocol, address) {
}

Client::~Client() = default;

void Client::input(const Task &task) {
    switch (state) {
        case GUEST: {
            break;
        }
        case LOGIN: {
            break;
        }
        case WAIT: {
            break;
        }
        case GAME: {
            break;
        }
    }
}

Task Client::output() {
    switch (state) {
        case GUEST: {

            break;
        }
        case LOGIN: {
            break;
        }
        case WAIT: {
            break;
        }
        case GAME: {
            break;
        }
    }
}

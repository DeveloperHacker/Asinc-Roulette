
#include "Server.h"

Server::Server(int domain, int type, int protocol, address_t &address)
        : TCPServer(domain, type, protocol, address) {
}

bool Server::handle(SafeSocket &socket) {

}

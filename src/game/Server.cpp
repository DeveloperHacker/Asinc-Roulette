
#include "Server.h"

Server::Server(int domain, int type, int protocol, address_t &address)
        : TCPServer(domain, type, protocol, address) {
}

bool Server::handle(const std::string &message, id_t id, SendSocket &socket) {
    return false;
}

void Server::connect_handle(id_t id) {

}

void Server::disconnect_handle(id_t id) {

}

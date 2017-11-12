
#include "Server.h"

Server::Server(int domain, int type, int protocol, address_t &address)
        : TCPServer(domain, type, protocol, address) {
}

bool Server::handle(id_t id, address_t address, const std::string &message) {
    return false;
}

void Server::connect_handle(id_t id, address_t address) {

}

void Server::disconnect_handle(id_t id, address_t address) {

}

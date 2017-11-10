#include "EchoServer.h"
#include "../global.h"


EchoServer::EchoServer(int domain, int type, int protocol, address_t &address
) : TCPServer(domain, type, protocol, address), clients() {
}

bool EchoServer::handle(const std::string &message, id_t id, SendSocket &socket) {
    auto &&client = clients.find(id);
    auto &&pair = client->second;
    auto &&transfer = pair.first;
    auto &&state = pair.second;
    switch (state) {
        case INIT: {
            state = READY;
            transfer->set_public_key(message);
            auto &&init = transfer->encrypt(global::transfer::INIT_MESSAGE);
            socket.send(init);
            return false;
        }
        case READY: {
            auto &&msg_input = transfer->decrypt(message);
            if (msg_input == "close")
                return true;
            std::string msg_output(msg_input.length(), 0);
            std::transform(msg_input.begin(), msg_input.end(), msg_output.begin(), ::toupper);
            auto &&enc_output = transfer->encrypt(msg_output);
            socket.send(enc_output);
            return false;
        }
    }
}

void EchoServer::connect_handle(id_t id) {
    auto &&transfer = std::make_shared<Transfer>();
    clients.emplace(id, std::make_pair(transfer, INIT));
}

void EchoServer::disconnect_handle(id_t id) {
    clients.erase(id);
}

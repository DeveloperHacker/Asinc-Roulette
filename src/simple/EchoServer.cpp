#include "EchoServer.h"


EchoServer::EchoServer(int domain, int type, int protocol, address_t &address
) : TCPServer(domain, type, protocol, address), state(Transfer::INIT) {
}

bool EchoServer::handle(const std::string &message, SendSocket &socket) {
    switch (state) {
        case Transfer::INIT: {
            state = Transfer::READY;
            transfer.set_public_key(message);
            auto &&init = transfer.encrypt(INIT_MESSAGE);
            socket.send(init);
            return false;
        }
        case Transfer::READY: {
            auto &&msg_input = transfer.decrypt(message);
            std::string msg_output(msg_input.length(), 0);
            std::transform(msg_input.begin(), msg_input.end(), msg_output.begin(), ::toupper);
            auto &&enc_output = transfer.encrypt(msg_output);
            socket.send(enc_output);
            return msg_input == "close";
        }
    }
}

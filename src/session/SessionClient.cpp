#include "SessionClient.h"
#include "config.h"
#include "../transfer/TransferServer.h"

SessionClient::SessionClient(std::shared_ptr<Socket> socket) : TransferClient(socket), state(INIT) {}

void SessionClient::output() {
    switch (state) {
        case INIT: {
            state = WAIT;
            raw_send(session.public_key());
            break;
        }
        case WAIT:
            break;
        case READY: {
            session_output();
            break;
        }
    }
}

void SessionClient::input(const std::string &message) {
    if (state == INIT)
        throw TransferClient::error("client must send rsa public key before receiving message");
#ifdef DEBUG_ENABLE
    std::cout << "[DEBUG] ERECV " << message << std::endl;
#endif
    auto &&decrypted = Session::unpack_and_decrypt_if_needed(session, message);
#ifdef DEBUG_ENABLE
    std::cout << "[DEBUG] DRECV " << decrypted << std::endl;
#endif
    switch (state) {
        case INIT:
            break;
        case WAIT: {
            state = READY;
            if (decrypted != crypto::INIT_MESSAGE)
                throw TransferClient::error("wrong initial message");
            break;
        }
        case READY: {
            session_input(decrypted);
            break;
        }
    }
}

void SessionClient::send(const char *message) {
    std::string m(message);
    send(m);
}

void SessionClient::send(const std::string &message) {
#ifdef DEBUG_ENABLE
    std::cout << "[DEBUG] MSEND " << message << std::endl;
#endif
    auto &&encrypted = Session::pack_and_encrypt_if_needed(session, message);
#ifdef DEBUG_ENABLE
    std::cout << "[DEBUG] ESEND " << encrypted << std::endl;
#endif
    TransferClient::send(encrypted);
}

void SessionClient::raw_send(const char *message) {
    std::string m(message);
    raw_send(m);
}

void SessionClient::raw_send(const std::string &message) {
#ifdef DEBUG_ENABLE
    std::cout << "[DEBUG] RSEND " << message << std::endl;
#endif
    TransferClient::send(crypto::RAW_MESSAGE_PREFIX + message);
}

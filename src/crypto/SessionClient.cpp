#include "SessionClient.h"
#include "config.h"
#include "../tcp/TCPServer.h"

CryptoClient::CryptoClient(int domain, int type, int protocol, address_t &address
) : TCPClient(domain, type, protocol, address), session(), state(INIT) {
}

void CryptoClient::output() {
    switch (state) {
        case INIT: {
            state = WAIT;
            raw_send(session.public_key());
            break;
        }
        case WAIT:
            break;
        case READY: {
            crypto_output();
            break;
        }
    }
}

void CryptoClient::input(const std::string &message) {
    if (state == INIT)
        throw TCPClient::error("client must send rsa public key before receiving message");
    std::cout << "[DEBUG] ERECV " << message << std::endl;
    auto &&decrypted = Session::unpack_and_decrypt_if_needed(session, message);
    std::cout << "[DEBUG] DRECV " << decrypted << std::endl;
    switch (state) {
        case INIT:
            break;
        case WAIT: {
            state = READY;
            if (decrypted != crypto::INIT_MESSAGE)
                throw TCPClient::error("wrong initial message");
            break;
        }
        case READY: {
            crypto_input(decrypted);
            break;
        }
    }
}

void CryptoClient::send(const char *message) {
    std::string m(message);
    send(m);
}

void CryptoClient::send(const std::string &message) {
    std::cout << "[DEBUG] MSEND " << message << std::endl;
    auto &&encrypted = Session::pack_and_encrypt_if_needed(session, message);
    std::cout << "[DEBUG] ESEND " << encrypted << std::endl;
    TCPClient::send(encrypted);
}

void CryptoClient::raw_send(const char *message) {
    std::string m(message);
    raw_send(m);
}

void CryptoClient::raw_send(const std::string &message) {
    TCPClient::send(crypto::RAW_MESSAGE_PREFIX + message);
}

#include "CryptoClient.h"
#include "config.h"
#include "../tcp/TCPServer.h"

CryptoClient::CryptoClient(int domain, int type, int protocol, address_t &address
) : TCPClient(domain, type, protocol, address), transfer(), state(INIT) {
}

void CryptoClient::output() {
    switch (state) {
        case INIT: {
            state = WAIT;
            raw_send(transfer.public_key());
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
    auto &&decrypted = Transfer::parse_and_decrypt_if_needed(transfer, message);
    switch (state) {
        case INIT:break;
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
    bool empty = message.empty();
    auto &&prefix = empty ? crypto::EMPTY_MESSAGE_PREFIX : crypto::NORMAL_MESSAGE_PREFIX;
    auto &&encrypted = transfer.encrypt(prefix + message);
    TCPClient::send(crypto::ENCRYPTED_MESSAGE_PREFIX + encrypted);
}

void CryptoClient::raw_send(const char *message) {
    std::string m(message);
    raw_send(m);
}

void CryptoClient::raw_send(const std::string &message) {
    TCPClient::send(crypto::RAW_MESSAGE_PREFIX + message);
}

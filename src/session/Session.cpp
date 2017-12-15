
#include <vector>
#include <iostream>
#include <memory>
#include "Session.h"
#include "config.h"
#include "strings.h"
#include "../../lib/base64/base64.h"

Session::Session() = default;

Session::Session(const std::string &public_key) {};

std::string Session::public_key() {
    std::string public_key(
            "---RSA KEY BEGIN---\n"
                    "STUBRSAKEY1023456789STUBRSAKEY1023456789STUBRSAKEY1023456789\n"
                    "STUBRSAKEY1023456789STUBRSAKEY1023456789STUBRSAKEY1023456789\n"
                    "STUBRSAKEY1023456789STUBRSAKEY1023456789STUBRSAKEY1023456789\n"
                    "STUBRSAKEY1023456789STUBRSAKEY1023456789STUBRSAKEY1023456789\n"
                    "STUBRSAKEY1023456789STUBRSAKEY1023456789STUBRSAKEY1023456789\n"
                    "---RSA KEY END---\n"
    );
    return public_key;
}

void Session::set_public_key(const std::string &key) {
    // do nothing
}

std::string Session::encrypt(const std::string &message) {
    auto &&c_message = reinterpret_cast<const unsigned char *>(message.c_str());
    auto &&c_length = static_cast<unsigned int>(message.length());
    return base64_encode(c_message, c_length);
}

std::string Session::decrypt(const std::string &message) {
    return base64_decode(message);
}

std::string Session::unpack(const std::string &message) {
    if (message.find(crypto::EMPTY_MESSAGE_PREFIX) != std::string::npos) {
        return strings::drop(message, crypto::EMPTY_MESSAGE_PREFIX.size());
    }
    if (message.find(crypto::NORMAL_MESSAGE_PREFIX) != std::string::npos) {
        return strings::drop(message, crypto::NORMAL_MESSAGE_PREFIX.size());
    }
    throw Session::error("undefined message correction " + message);
}

std::string Session::unpack_and_decrypt_if_needed(Session &transfer, const std::string &message) {
    if (message.find(crypto::RAW_MESSAGE_PREFIX) != std::string::npos) {
        return strings::drop(message, crypto::RAW_MESSAGE_PREFIX.size());
    }
    if (message.find(crypto::ENCRYPTED_MESSAGE_PREFIX) != std::string::npos) {
        auto &&encrypted = strings::drop(message, crypto::ENCRYPTED_MESSAGE_PREFIX.size());
        return unpack(transfer.decrypt(encrypted));
    }
    throw Session::error("undefined message type " + message);
}

std::string Session::pack_and_encrypt_if_needed(Session &transfer, const std::string &message) {
    bool empty = message.empty();
    auto &&prefix = empty ? crypto::EMPTY_MESSAGE_PREFIX : crypto::NORMAL_MESSAGE_PREFIX;
    return crypto::ENCRYPTED_MESSAGE_PREFIX + transfer.encrypt(prefix + message);
}

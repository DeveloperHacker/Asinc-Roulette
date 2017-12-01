
#include <vector>
#include <iostream>
#include <memory>
#include "Transfer.h"
#include "config.h"
#include "strings.h"
#include "../../lib/base64/base64.h"

Transfer::Transfer() = default;

Transfer::Transfer(const std::string &public_key) {};

std::string Transfer::public_key() {
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

void Transfer::set_public_key(const std::string &key) {
    // do nothing
}

std::string Transfer::encrypt(const std::string &message) {
    auto &&c_message = reinterpret_cast<const unsigned char *>(message.c_str());
    auto &&c_length = static_cast<unsigned int>(message.length());
    return base64_encode(c_message, c_length);
}

std::string Transfer::decrypt(const std::string &message) {
    return base64_decode(message);
}

std::string Transfer::unpack(const std::string &message) {
    if (message.find(crypto::EMPTY_MESSAGE_PREFIX) != std::string::npos) {
        return strings::drop(message, crypto::EMPTY_MESSAGE_PREFIX.size());
    }
    if (message.find(crypto::NORMAL_MESSAGE_PREFIX) != std::string::npos) {
        return strings::drop(message, crypto::NORMAL_MESSAGE_PREFIX.size());
    }
    throw Transfer::error("undefined message correction " + message);
}

std::string Transfer::unpack_and_decrypt_if_needed(Transfer &transfer, const std::string &message) {
    if (message.find(crypto::RAW_MESSAGE_PREFIX) != std::string::npos) {
        return strings::drop(message, crypto::RAW_MESSAGE_PREFIX.size());
    }
    if (message.find(crypto::ENCRYPTED_MESSAGE_PREFIX) != std::string::npos) {
        auto &&encrypted = strings::drop(message, crypto::ENCRYPTED_MESSAGE_PREFIX.size());
        return unpack(transfer.decrypt(encrypted));
    }
    throw Transfer::error("undefined message type " + message);
}

std::string Transfer::pack_and_encrypt_if_needed(Transfer &transfer, const std::string &message) {
    bool empty = message.empty();
    auto &&prefix = empty ? crypto::EMPTY_MESSAGE_PREFIX : crypto::NORMAL_MESSAGE_PREFIX;
    return crypto::ENCRYPTED_MESSAGE_PREFIX + transfer.encrypt(prefix + message);
}

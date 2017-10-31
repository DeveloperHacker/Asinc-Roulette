
#include "Transfer.h"
#include "../lib/crypto/base64.h"
#include <stdexcept>
#include <iostream>

Transfer::Transfer() : state(MASTER), crypto() {};

Transfer::Transfer(const std::string &key) : state(SLAVE), crypto() {
    auto *c_key = new unsigned char[key.length()];
    memcpy(c_key, key.c_str(), key.length());
    crypto.setRemotePublicKey(c_key, key.length());
    delete c_key;
}

std::string Transfer::rsa_key() {
    unsigned char *public_key;
    if (FAILURE == crypto.getRemotePublicKey(&public_key)) {
        throw Transfer::error("Allocation error");
    }
    std::string result(reinterpret_cast<char *>(public_key));
    free(public_key);
    return result;
}

std::string Transfer::rsa_encrypt(const std::string &message) {
    unsigned char *encrypted, *key, *iv;
    size_t key_length, iv_length;
    auto *data = (unsigned char *) message.c_str();
    size_t data_length = message.length();
    auto &&encrypted_length = crypto.rsaEncrypt(data, data_length, &encrypted, &key, &key_length, &iv, &iv_length);

}

std::string Transfer::rsa_decrypt(const std::string &message) {

}

std::string Transfer::aes_encrypt(const std::string &message) {

}

std::string Transfer::aes_decrypt(const std::string &message) {

}

std::string Transfer::decrypt(const std::string &message) {
    unsigned char *encrypted, *decrypted;
    auto encrypted_length = base64Decode(message.c_str(), message.length(), &encrypted);
    auto decrypted_length = crypto.aesDecrypt(encrypted, static_cast<size_t>(encrypted_length), &decrypted);
    if (decrypted_length == FAILURE) throw Transfer::error("Decryption failed");
    std::string result(reinterpret_cast<char *>(decrypted), static_cast<unsigned long>(decrypted_length));
    free(encrypted);
    free(decrypted);
    return result;
}

std::string Transfer::encrypt(const std::string &message) {
    unsigned char *encrypted;
    auto &&c_message = reinterpret_cast<const unsigned char *>(message.c_str());
    auto &&encrypted_length = crypto.aesEncrypt(c_message, message.size() + 1, &encrypted);
    if (encrypted_length == FAILURE) throw Transfer::error("Encryption fail");
    auto *b64_encrypted = base64Encode(encrypted, static_cast<const size_t>(encrypted_length));
    std::string result(b64_encrypted);
    free(encrypted);
    free(b64_encrypted);
    return result;
}

std::string Transfer::encrypt(const char *message) {
    std::string data(message);
    return encrypt(data);
}

std::string Transfer::decrypt(const char *message) {
    std::string data(message);
    return decrypt(data);
}

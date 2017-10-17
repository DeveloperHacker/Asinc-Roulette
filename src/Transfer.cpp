
#include "Transfer.h"
#include "../lib/crypto/base64.h"
#include <stdexcept>
#include <iostream>

class Transfer::error : public std::runtime_error {
    using std::runtime_error::runtime_error;
public:
    ~error() override = default;
};

Transfer::Transfer() : type(MASTER) {};

Transfer::Transfer(const std::string &key) : type(SLAVE) {
    unsigned char *encrypted;
    auto &&encrypted_length = base64Decode(key.c_str(), key.length(), &encrypted);
    std::cout << encrypted_length << std::endl;
    std::cout << encrypted <<  std::endl;
    crypto = Crypto(encrypted, static_cast<size_t>(encrypted_length));
    free(encrypted);
}

std::string Transfer::rsa_key() {
    unsigned char *public_key;
    auto &&success = crypto.getRemotePublicKey(&public_key);
    if (success == FAILURE) throw Transfer::error("Allocation error");
    std::cout << public_key << std::endl;
    auto *b64_key = base64Encode(public_key, strlen(reinterpret_cast<const char *>(public_key)));
    std::string result(b64_key);
    free(public_key);
    free(b64_key);
    return result;
}

std::string Transfer::aes_key() {
//    if (type == MASTER) throw Transfer::error("Slave must generate rsa public key");
    unsigned char *key;
    auto &&key_length = crypto.getAesKey(&key);
    // encrypt {
    unsigned char *encrypted = key;
    auto &&encrypted_length = key_length;
    for (int i = 0; i < encrypted_length; ++i) std::cout << (int) encrypted[i] << " ";
    std::cout << std::endl;
    // }
    auto *b64_encrypted = base64Encode(encrypted, static_cast<const size_t>(encrypted_length));
    std::string result(b64_encrypted);
    free(encrypted);
    free(b64_encrypted);
    return result;
}

void Transfer::aes_key(const std::string &key) {
    if (type == SLAVE) throw Transfer::error("Slave must generate rsa public key");
    unsigned char *encrypted, *decrypted;
    auto encrypted_length = base64Decode(key.c_str(), key.length(), &encrypted);
    for (int i = 0; i < encrypted_length; ++i) std::cout << (int) encrypted[i] << " ";
    std::cout << std::endl;
    // encrypt {
    decrypted = encrypted;
    auto &&decrypted_length = encrypted_length;
    // }
    crypto.setAesKey(decrypted, static_cast<size_t>(decrypted_length));
    free(encrypted);
    free(decrypted);
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

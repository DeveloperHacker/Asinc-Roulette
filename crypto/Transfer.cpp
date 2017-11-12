
#include <vector>
#include <iostream>
#include <memory>
#include "Transfer.h"
#include "../lib/crypto/base64.h"
#include "config.h"
#include "strings.h"

Transfer::Transfer() : state(INIT), crypto() {};

Transfer::Transfer(const std::string &public_key) : state(INIT), crypto() {
    set_public_key(public_key);
}

std::string Transfer::public_key() {
    unsigned char *public_key;
    auto &&status = crypto.getRemotePublicKey(&public_key);
    if (status == FAILURE) throw Transfer::error("RSA key generation fail");
    std::string result(reinterpret_cast<char *>(public_key));
    free(public_key);
    return result;
}

void Transfer::set_public_key(const std::string &key) {
    auto *c_key = new unsigned char[key.length()];
    memcpy(c_key, key.c_str(), key.length());
    crypto.setRemotePublicKey(c_key, key.length());
    delete c_key;
}

class Vector {
public:
    const size_t data_length;
    const size_t key_length;
    const size_t iv_length;
    unsigned char *data;
    unsigned char *key;
    unsigned char *iv;

    Vector(size_t data_length,
           size_t key_length,
           size_t iv_length,
           unsigned char *data,
           unsigned char *key,
           unsigned char *iv
    ) : data_length(data_length),
        key_length(key_length),
        iv_length(iv_length),
        data(data),
        key(key),
        iv(iv) {
    }

    Vector(Vector &&vector) = default;

    ~Vector() {
        free(data);
        free(key);
        free(iv);
    }
};

std::vector<unsigned char> construct(const std::shared_ptr<Vector> &vector) {
    auto type_size = sizeof(size_t);
    auto size = 3 * type_size + vector->data_length + vector->key_length + vector->iv_length;
    std::vector<unsigned char> result(size, 0);
    unsigned char *data = result.data();
    auto offset = 0;
    memcpy(data + offset, &vector->data_length, type_size);
    offset += type_size;
    memcpy(data + offset, &vector->key_length, type_size);
    offset += type_size;
    memcpy(data + offset, &vector->iv_length, type_size);
    offset += type_size;
    memcpy(data + offset, vector->data, vector->data_length);
    offset += vector->data_length;
    memcpy(data + offset, vector->key, vector->key_length);
    offset += vector->key_length;
    memcpy(data + offset, vector->iv, vector->iv_length);
    return result;
}

std::shared_ptr<Vector> deconstruct(const unsigned char *vector, size_t length) {
    auto type_size = sizeof(size_t);
    size_t data_length, key_length, iv_length, offset = 0;
    if (offset + type_size >= length) throw Transfer::error("wrong message format");
    memcpy(&data_length, vector + offset, type_size);
    offset += type_size;
    if (offset + type_size >= length) throw Transfer::error("wrong message format");
    memcpy(&key_length, vector + offset, type_size);
    offset += type_size;
    if (offset + type_size >= length) throw Transfer::error("wrong message format");
    memcpy(&iv_length, vector + offset, type_size);
    offset += type_size;
    auto size = 3 * type_size + data_length + key_length + iv_length;
    if (size != length) throw Transfer::error("wrong message format");
    auto &&data = (unsigned char *) malloc(data_length);
    auto &&key = (unsigned char *) malloc(key_length);
    auto &&iv = (unsigned char *) malloc(iv_length);
    memcpy(data, vector + offset, data_length);
    offset += data_length;
    memcpy(key, vector + offset, key_length);
    offset += key_length;
    memcpy(iv, vector + offset, iv_length);
    return std::make_shared<Vector>(data_length, key_length, iv_length, data, key, iv);
}


std::vector<char> Transfer::rsa_encrypt(const std::vector<unsigned char> &message) {
    unsigned char *encrypted, *key, *iv;
    size_t key_len = 0, iv_len = 0;
    auto length = crypto.rsaEncrypt(message.data(), message.size(), &encrypted, &key, &key_len, &iv, &iv_len);
    if (length == FAILURE) throw Transfer::error("RSA encryption fail");
    auto &&v = std::make_shared<Vector>(length, key_len, iv_len, encrypted, key, iv);
    auto &&constructed = construct(v);
    auto *b64 = base64Encode(constructed.data(), constructed.size());
    auto b64_length = strlen(b64) + 1;
    std::vector<char> result(b64_length, 0);
    memcpy(result.data(), b64, b64_length);
    free(b64);
    return result;
}

std::vector<unsigned char> Transfer::rsa_decrypt(const std::vector<char> &message) {
    unsigned char *b64, *decrypted;
    auto b64_length = base64Decode(message.data(), message.size(), &b64);
    auto &&v = deconstruct(b64, b64_length);
    if (b64_length <= 0) throw Transfer::error("Base 64 decode failed");
    auto length = crypto.rsaDecrypt(v->data, v->data_length, v->key, v->key_length, v->iv, v->iv_length, &decrypted);
    if (length == FAILURE) throw Transfer::error("RSA decryption failed");
    std::vector<unsigned char> result(length, 0);
    memcpy(result.data(), decrypted, static_cast<size_t>(length));
    free(decrypted);
    free(b64);
    return result;
}

std::vector<char> Transfer::aes_encrypt(const std::vector<unsigned char> &message) {
    unsigned char *encrypted;
    auto length = crypto.aesEncrypt(message.data(), message.size() + 1, &encrypted);
    if (length == FAILURE) throw Transfer::error("AES encryption fail");
    auto *b64 = base64Encode(encrypted, static_cast<const size_t>(length));
    auto b64_length = strlen(b64) + 1;
    std::vector<char> result(b64_length, 0);
    memcpy(result.data(), b64, b64_length);
    free(encrypted);
    free(b64);
    return result;
}

std::vector<unsigned char> Transfer::aes_decrypt(const std::vector<char> &message) {
    unsigned char *b64, *decrypted;
    auto b64_length = base64Decode(message.data(), message.size(), &b64);
    if (b64_length <= 0) throw Transfer::error("Base 64 decode failed");
    auto length = crypto.aesDecrypt(b64, static_cast<size_t>(b64_length), &decrypted) - 1;
    if (length == FAILURE) throw Transfer::error("AES decryption failed");
    std::vector<unsigned char> result(length, 0);
    memcpy(result.data(), decrypted, static_cast<size_t>(length));
    free(decrypted);
    free(b64);
    return result;
}

std::vector<unsigned char> Transfer::decrypt(const std::vector<char> &message) {
    if (state == INIT) {
        state = READY;
        auto &&key = rsa_decrypt(message);
        return slave_init(key);
    } else {
        return aes_decrypt(message);
    }
}

std::vector<char> Transfer::encrypt(const std::vector<unsigned char> &message) {
    if (state == INIT) {
        state = READY;
        auto &&key = master_init(message);
        return rsa_encrypt(key);
    } else {
        return aes_encrypt(message);
    }
}

std::string Transfer::decrypt(const std::string &message) {
    std::vector<char> data(std::begin(message), std::end(message));
    std::vector<unsigned char> decrypted = decrypt(data);
    return std::string(reinterpret_cast<char *>(decrypted.data()), decrypted.size());
}

std::string Transfer::encrypt(const std::string &message) {
    std::vector<unsigned char> data(std::begin(message), std::end(message));
    std::vector<char> encrypted = encrypt(data);
    return std::string(encrypted.data(), encrypted.size());
}

std::string Transfer::encrypt(const char *message) {
    std::string data(message);
    return encrypt(data);
}

std::string Transfer::decrypt(const char *message) {
    std::string data(message);
    return decrypt(data);
}

std::vector<unsigned char> Transfer::master_init(const std::vector<unsigned char> &message) {
    unsigned char *key_value, *iv_value;
    auto key_length = static_cast<size_t>(crypto.getAesKey(&key_value));
    auto iv_length = static_cast<size_t>(crypto.getAesIv(&iv_value));
    auto data_length = message.size();
    auto &&data = new unsigned char[data_length];
    auto &&key = new unsigned char[key_length];
    auto &&iv = new unsigned char[iv_length];
    memcpy(data, message.data(), data_length);
    memcpy(key, key_value, key_length);
    memcpy(iv, iv_value, iv_length);
    auto &&vector = std::make_shared<Vector>(data_length, key_length, iv_length, data, key, iv);
    return construct(vector);
}

std::vector<unsigned char> Transfer::slave_init(const std::vector<unsigned char> &key) {
    auto &&vector = deconstruct(key.data(), key.size());
    crypto.setAesKey(vector->key, vector->key_length);
    crypto.setAesIv(vector->iv, vector->iv_length);
    std::vector<unsigned char> message(vector->data_length, 0);
    memcpy(message.data(), vector->data, vector->data_length);
    return message;
}

std::string Transfer::unpack(const std::string &message) {
    if (message.find(crypto::EMPTY_MESSAGE_PREFIX) == 0) {
        return strings::drop(message, crypto::EMPTY_MESSAGE_PREFIX.size());
    }
    if (message.find(crypto::NORMAL_MESSAGE_PREFIX) == 0) {
        return strings::drop(message, crypto::NORMAL_MESSAGE_PREFIX.size());
    }
    throw Transfer::error("undefined message correction " + message);
}

std::string Transfer::parse_and_decrypt_if_needed(Transfer &transfer, const std::string &message) {
    if (message.find(crypto::RAW_MESSAGE_PREFIX) == 0) {
        return strings::drop(message, crypto::RAW_MESSAGE_PREFIX.size());
    }
    if (message.find(crypto::ENCRYPTED_MESSAGE_PREFIX) == 0) {
        auto &&encrypted = strings::drop(message, crypto::ENCRYPTED_MESSAGE_PREFIX.size());
        return unpack(transfer.decrypt(encrypted));
    }
    throw Transfer::error("undefined message type " + message);
}

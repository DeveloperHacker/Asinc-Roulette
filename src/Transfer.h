#pragma once

#include "../lib/crypto/Crypto.h"
#include <string>

enum Type {
    MASTER, SLAVE
};

enum State {
    RSA, AES
};


class Transfer {
public:
    class error;

private:
    Crypto crypto;
    Type type;
    State state;

public:
    Transfer();

    explicit Transfer(const std::string &key);

    std::string rsa_key();

    std::string aes_key();

    void aes_key(const std::string &key);

    std::string encrypt(const std::string &message);

    std::string decrypt(const std::string &message);

    std::string encrypt(const char *message);

    std::string decrypt(const char *message);

private:
    std::string Transfer::rsa_encrypt(const std::string &message);

    std::string Transfer::rsa_decrypt(const std::string &message);

    std::string Transfer::aes_encrypt(const std::string &message);

    std::string Transfer::aes_decrypt(const std::string &message);
};

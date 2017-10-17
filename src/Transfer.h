#pragma once

#include "../lib/crypto/Crypto.h"
#include <string>

class Transfer {
private:
    enum Type {
        MASTER, SLAVE
    };

public:
    class error;

private:
    Crypto crypto;
    Type type;

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
};

#pragma once

#include "../lib/crypto/Crypto.h"
#include <string>
#include <stdexcept>

enum Type {
    MASTER, SLAVE
};

class Transfer {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

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

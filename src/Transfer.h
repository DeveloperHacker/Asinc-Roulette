#pragma once

#include "../lib/crypto/Crypto.h"
#include <string>
#include <stdexcept>

enum State {
    MASTER,
    SLAVE,
    EQUALITY
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
    State state;

public:
    Transfer();

    explicit Transfer(const std::string &key);

    std::string rsa_key();

    std::string encrypt(const std::string &message);

    std::string decrypt(const std::string &message);

    std::string encrypt(const char *message);

    std::string decrypt(const char *message);

private:
    std::string rsa_encrypt(const std::string &message);

    std::string rsa_decrypt(const std::string &message);

    std::string aes_encrypt(const std::string &message);

    std::string aes_decrypt(const std::string &message);
};

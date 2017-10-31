#pragma once

#include "../lib/crypto/Crypto.h"
#include <string>
#include <vector>
#include <stdexcept>

enum State {
    INIT,
    READY
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

    explicit Transfer(const std::string &public_key);

    std::string public_key();

    std::vector<char> encrypt(const std::vector<unsigned char> &message);

    std::vector<unsigned char> decrypt(const std::vector<char> &message);

    std::string encrypt(const std::string &message);

    std::string decrypt(const std::string &message);

    std::string encrypt(const char *message);

    std::string decrypt(const char *message);

private:
    std::vector<unsigned char> master_init(const std::vector<unsigned char> &message);

    std::vector<unsigned char> slave_init(const std::vector<unsigned char> &key);

    std::vector<char> rsa_encrypt(const std::vector<unsigned char> &message);

    std::vector<unsigned char> rsa_decrypt(const std::vector<char> &message);

    std::vector<char> aes_encrypt(const std::vector<unsigned char> &message);

    std::vector<unsigned char> aes_decrypt(const std::vector<char> &message);
};

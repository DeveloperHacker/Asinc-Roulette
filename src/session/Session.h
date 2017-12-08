#pragma once

#include "crypto/Crypto.h"
#include <string>
#include <vector>
#include <stdexcept>


class Session {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

    enum State {
        INIT,
        READY
    };

private:
    Crypto crypto;
    State state;

public:
    Session();

    explicit Session(const std::string &public_key);

    std::string public_key();

    void set_public_key(const std::string &key);

    std::string encrypt(const std::string &message);

    std::string decrypt(const std::string &message);

private:
    std::vector<unsigned char> master_init(const std::vector<unsigned char> &message);

    std::vector<unsigned char> slave_init(const std::vector<unsigned char> &key);

    std::string rsa_encrypt(const std::vector<unsigned char> &message);

    std::vector<unsigned char> rsa_decrypt(const std::string &message);

    std::string aes_encrypt(const std::vector<unsigned char> &message);

    std::vector<unsigned char> aes_decrypt(const std::string &message);

public:
    static std::string pack_and_encrypt_if_needed(Session &session, const std::string &message);

    static std::string unpack_and_decrypt_if_needed(Session &session, const std::string &message);

    static std::string unpack(const std::string &message);
};

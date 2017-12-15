#pragma once

#include <string>
#include <vector>

class Session {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

public:
    Session();

    explicit Session(const std::string &public_key);

    std::string public_key();

    void set_public_key(const std::string &key);

    std::string encrypt(const std::string &message);

    std::string decrypt(const std::string &message);

public:
    static std::string pack_and_encrypt_if_needed(Session &transfer, const std::string &message);

    static std::string unpack_and_decrypt_if_needed(Session &transfer, const std::string &message);

    static std::string unpack(const std::string &message);
};

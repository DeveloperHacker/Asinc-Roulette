#pragma once

#include <string>
#include <vector>

class Transfer {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

public:
    Transfer();

    explicit Transfer(const std::string &public_key);

    std::string public_key();

    void set_public_key(const std::string &key);

    std::string encrypt(const std::string &message);

    std::string decrypt(const std::string &message);

public:
    static std::string pack_and_encrypt_if_needed(Transfer &transfer, const std::string &message);

    static std::string unpack_and_decrypt_if_needed(Transfer &transfer, const std::string &message);

    static std::string unpack(const std::string &message);
};

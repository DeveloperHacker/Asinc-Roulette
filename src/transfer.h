#pragma once

#include <string>
#include <stdexcept>
#include <array>
#include <algorithm>

namespace transfer {

    class error;


    const size_t RSA_KEY_LENGTH = 256;

    const size_t AES_KEY_LENGTH = 256;

    const size_t BLOCK_LENGTH = 256;


    std::string aes_key();

    std::string aes_encrypt(const std::string &data, const std::string &key);

    std::string aes_decrypt(const std::string &data, const std::string &key);


    std::string rsa_private_key();

    std::string rsa_public_key(const std::string &private_key);

    std::string rsa_encrypt(const std::string &data, const std::string &key);

    std::string rsa_decrypt(const std::string &data, const std::string &key);
}
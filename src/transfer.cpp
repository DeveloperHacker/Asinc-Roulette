#include "transfer.h"

class transfer::error : public std::runtime_error {
    using std::runtime_error::runtime_error;
public:
    ~error() override = default;
};

std::string aes_key() {

}

std::string aes_encrypt(const std::string &data, const std::string &key) {

}

std::string aes_decrypt(const std::string &data, const std::string &key) {

}

std::string rsa_private_key() {

}

std::string rsa_public_key(const std::string &private_key) {

}

std::string rsa_encrypt(const std::string &data, const std::string &public_key) {

}

std::string rsa_decrypt(const std::string &data, const std::string &private_key) {

}

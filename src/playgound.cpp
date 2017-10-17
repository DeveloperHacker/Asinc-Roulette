
#include <iostream>
#include "Transfer.h"

int main() {
    Transfer master;
    auto &&rsa_key = master.rsa_key();
    std::cout << "Rsa key          = " << rsa_key << std::endl;
    Transfer slave(rsa_key);
    auto &&aes_key = slave.aes_key();
    std::cout << "Aes key          = " << aes_key << std::endl;
    master.aes_key(aes_key);
    auto &&encrypt_request = slave.encrypt("request");
    std::cout << "Encrypt request  = " << encrypt_request << std::endl;
    auto &&decrypt_request = master.decrypt(encrypt_request);
    std::cout << "Decrypt request  = " << decrypt_request << std::endl;
    auto &&encrypt_response = master.encrypt("response");
    std::cout << "Encrypt response = " << encrypt_response << std::endl;
    auto &&decrypt_response = slave.encrypt(encrypt_response);
    std::cout << "Decrypt response = " << decrypt_response << std::endl;
    return 0;
}

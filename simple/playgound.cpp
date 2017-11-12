
#include <iostream>
#include "../crypto/Transfer.h"
#include "../crypto/strings.h"
#include "../crypto/config.h"

void slave2master(Transfer &master, Transfer &slave) {
    auto &&inner_prefix = crypto::NORMAL_MESSAGE_PREFIX;
    auto &&outer_prefix = crypto::ENCRYPTED_MESSAGE_PREFIX;
    std::string message("transfer slave to master");

    auto &&encrypt_response = outer_prefix + slave.encrypt(inner_prefix + message);
    std::cout << "Encrypt response = " << encrypt_response << std::endl;
    auto &&decrypt_response = master.decrypt(strings::drop(encrypt_response, outer_prefix.size()));
    std::cout << "Decrypt response = " << strings::drop(decrypt_response, inner_prefix.size()) << std::endl;
}


void master2slave(Transfer &master, Transfer &slave) {
    auto &&inner_prefix = crypto::NORMAL_MESSAGE_PREFIX;
    auto &&outer_prefix = crypto::ENCRYPTED_MESSAGE_PREFIX;
    std::string message("transfer master to slave");

    auto &&encrypt_response = outer_prefix + master.encrypt(inner_prefix + message);
    std::cout << "Encrypt response = " << encrypt_response << std::endl;
    auto &&decrypt_response = slave.decrypt(strings::drop(encrypt_response, outer_prefix.size()));
    std::cout << "Decrypt response = " << strings::drop(decrypt_response, inner_prefix.size()) << std::endl;
}

void transfer_test(int counts) {
    auto &&prefix = crypto::RAW_MESSAGE_PREFIX;
    Transfer master;
    auto &&rsa_key = prefix + master.public_key();
    std::cout << rsa_key << std::endl;
    Transfer slave(strings::drop(rsa_key, prefix.size()));
    for (int i = 0; i < counts; ++i) {
        slave2master(master, slave);
        master2slave(master, slave);
    }
}

int main() {
    for (int i = 0; i < 10; ++i) {
        transfer_test(10);
    }
    return 0;
}

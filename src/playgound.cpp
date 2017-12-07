
#include <iostream>
#include "crypto/Transfer.h"
#include "crypto/strings.h"
#include "crypto/config.h"

std::string random_string() {
    std::string string;
    auto &&length = std::rand() % 100;
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; ++i) {
        auto &&ch = alphanum[std::rand() % (sizeof(alphanum) - 1)];
        string.push_back(ch);
    }
    return string;
}

void slave2master(Transfer &master, Transfer &slave) {
    auto &&inner_prefix = crypto::NORMAL_MESSAGE_PREFIX;
    auto &&outer_prefix = crypto::ENCRYPTED_MESSAGE_PREFIX;
    auto &&message = random_string();
    std::cout << "Message response = " << message << std::endl;
    auto &&encrypt_response = Transfer::pack_and_encrypt_if_needed(slave, message);
    std::cout << "Encrypt response = " << encrypt_response << std::endl;
    auto &&decrypt_response = Transfer::unpack_and_decrypt_if_needed(master, encrypt_response);
    std::cout << "Decrypt response = " << decrypt_response << std::endl;
}


void master2slave(Transfer &master, Transfer &slave) {
    auto &&inner_prefix = crypto::NORMAL_MESSAGE_PREFIX;
    auto &&outer_prefix = crypto::ENCRYPTED_MESSAGE_PREFIX;
    auto &&message = random_string();
    std::cout << "Message request  = " << message << std::endl;
    auto &&encrypt_response = Transfer::pack_and_encrypt_if_needed(master, message);
    std::cout << "Encrypt request  = " << encrypt_response << std::endl;
    auto &&decrypt_response = Transfer::unpack_and_decrypt_if_needed(slave, encrypt_response);
    std::cout << "Decrypt request  = " << decrypt_response << std::endl;
}

void transfer_test(int counts) {
    auto &&prefix = crypto::RAW_MESSAGE_PREFIX;
    Transfer master;
    auto &&rsa_key = prefix + master.public_key();
    std::cout << rsa_key;
    Transfer slave(strings::drop(rsa_key, prefix.size()));
    for (int i = 0; i < counts; ++i) {
        slave2master(master, slave);
        master2slave(master, slave);
    }
}

int main() {
    std::srand(std::time(nullptr));
    for (int i = 0; i < 10; ++i) {
        transfer_test(10);
    }
    return 0;
}

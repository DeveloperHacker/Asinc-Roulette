#pragma once

#include <string>
#include "../lib/biginteger.h"

namespace encryption {
    // RSA
    BigInteger encrypt(const BigInteger &data, const BigInteger &public_key) {

    }

    // AES
    std::string encrypt(const std::string &data, const BigInteger &) {

    }
};

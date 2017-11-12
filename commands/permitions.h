#pragma once

#include <cstdint>

using permition_t = uint8_t;

namespace permitions {
    static const permition_t NONE = 0;  // clients without commands
    static const permition_t GUEST = 1;  // clients with guest commands
    static const permition_t USER = 4; // clients with user commands
    static const permition_t STAFF = 8; // clients with staff commands
    static const permition_t PLAYER = 16; // clients at table with player commands
    static const permition_t CROUPIER = 32; // clients at table with croupier commands
    static const permition_t ADMIN = 64;  // clients with admin commands
    static const permition_t AUTH = USER | STAFF | ADMIN; // clients authenticated in system
    static const permition_t PARTY = PLAYER | CROUPIER; // clients at same table
    static const permition_t ALL = NONE | GUEST | USER | STAFF | PLAYER | CROUPIER | ADMIN;
};

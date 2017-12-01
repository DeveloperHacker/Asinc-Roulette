#pragma once

#include <cstdint>

using permission_t = int;

namespace permissions {
    static const permission_t NONE = 0;  // clients without commands
    static const permission_t GUEST = 1;  // clients with guest commands
    static const permission_t USER = 4; // clients with user commands
    static const permission_t STAFF = 8; // clients with staff commands
    static const permission_t PLAYER = 16; // clients at table with player commands
    static const permission_t CROUPIER = 32; // clients at table with croupier commands
    static const permission_t ADMIN = 64;  // clients with admin commands
    static const permission_t WAIT = USER | STAFF | ADMIN; // clients authenticated in system
    static const permission_t PLAY = PLAYER | CROUPIER; // clients at same table
    static const permission_t AUTH = WAIT | PLAY; // clients at same table
    static const permission_t ALL = NONE | GUEST | USER | STAFF | PLAYER | CROUPIER | ADMIN;
};

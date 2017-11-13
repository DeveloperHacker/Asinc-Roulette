#pragma once

#include <string>
#include <stdint-gcc.h>

namespace address {
    const std::string SERVER_HOST("127.0.0.1"); // NOLINT
    const uint16_t SERVER_PORT = 7777;
}

namespace parts {
    const std::string STATUS("code"); // NOLINT
    const std::string COMMAND("command"); // NOLINT
    const std::string DATA("data"); // NOLINT
    const std::string MESSAGE("message"); // NOLINT
    const std::string LOGIN("login"); // NOLINT
    const std::string PASSWORD("password"); // NOLINT
    const std::string NAME("name"); // NOLINT
    const std::string ROLE("role"); // NOLINT
    const std::string PERMITION("permition"); // NOLINT
    const std::string LIST("list"); // NOLINT
    const std::string NUM_PLAYERS("num_players"); // NOLINT
    const std::string MAX_PLAYERS("max_players"); // NOLINT
    const std::string LOCK("lock"); // NOLINT
}

namespace stats {
    const std::string RESOLVED("success"); // NOLINT
    const std::string ERROR("error"); // NOLINT
}

namespace commands {
    const std::string LOGIN("login"); // NOLINT
    const std::string LOGOUT("logout"); // NOLINT
    const std::string JOIN("join"); // NOLINT
    const std::string CREATE("create"); // NOLINT
    const std::string LEAVE("leave"); // NOLINT
    const std::string TABLES("list"); // NOLINT
    const std::string USERS("users"); // NOLINT
    const std::string WRITE("write"); // NOLINT
    const std::string DISCONNECT("disconnect"); // NOLINT
    const std::string SYNC("sync"); // NOLINT
    const std::string REGISTRATION("registration"); // NOLINT
    const std::string SET_PERMITION("set_permition"); // NOLINT
}

namespace other {
    const std::string GUEST("guest"); // NOLINT
    const std::string CROUPIER("croupier"); // NOLINT
    const std::string PLAYER("player"); // NOLINT
    const size_t MAX_PLAYERS = 5; // NOLINT
    const std::string DATA_BASE("data_base.db"); // NOLINT
}

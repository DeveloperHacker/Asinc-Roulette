#pragma once

#include "json/src/json.hpp"
#include <memory>
#include <fstream>

using json = nlohmann::json;

class Config {
private:
    static bool inited;
    static std::string host;
    static uint16_t port;

public:
    static std::string get_host();

    static uint16_t get_port();

    static void init();
};
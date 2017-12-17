#include "Config.h"

bool Config::inited = false;

std::string Config::host("");

uint16_t Config::port = 0;

std::string Config::get_host() {
    if (!inited)
        init();
    return host;
}

uint16_t Config::get_port() {
    if (!inited)
        init();
    return port;
}

void Config::init() {
    std::ifstream stream("config.json");
    json config;
    stream >> config;
    host = config["host"];
    port = config["port"];
    inited = true;
}

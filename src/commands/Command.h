#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <iostream>
#include "../../src/crypto/strings.h"

class Command {
private:
    std::string name;
    std::vector<std::string> arguments;

public:
    explicit Command(const std::string &command);

    ~Command() = default;

    std::string get_name() const;

    std::vector<std::string> get_arguments() const;

    std::string get_argument(size_t index) const;

    static std::vector<std::string> parse(const std::string &command);
};


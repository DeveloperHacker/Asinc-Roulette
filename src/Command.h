#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <iostream>
#include "strings.h"

class Command {
private:
    std::string name;
    std::vector<std::string> arguments;

public:
    explicit Command(const std::string &command) {
        auto &&vector = parse(command);
        name = vector[0];
        auto &&begin = vector.begin() + 1;
        auto &&end = vector.end();
        arguments = std::vector<std::string>(begin, end);
    }

    ~Command() = default;

    std::string get_name() {
        return name;
    }

    std::vector<std::string> get_arguments() {
        return arguments;
    }

    std::string get_argument(size_t index) {
        return arguments[index];
    }

    static std::vector<std::string> parse(const std::string &command) {
        std::vector<std::string> result;
        std::string current;
        bool in_string = false;
        char quote = ' ';
        for (auto ch : utils::trim_copy(command)) {
            if (in_string && ch == quote) {
                in_string = false;
            } else if (!in_string && (ch == '\'' || ch == '"')) {
                in_string = true;
                quote = ch;
            } else if (!in_string && ch == ' ') {
                result.push_back(current);
                current.clear();
            } else {
                current.push_back(ch);
            }
        }
        result.push_back(current);
        return result;
    }
};


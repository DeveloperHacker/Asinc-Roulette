#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <iostream>

class Command {
private:
    std::string name;
    std::vector<std::string> arguments;

public:
    explicit Command(const std::string &command) {
        auto &&vector = Command::split(command, ' ');
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

    static std::vector<std::string> split(const std::string &string, char delimiter) {
        std::vector<std::string> result;
        std::string current;
        for (auto &&ch : string) {
            if (ch == delimiter) {
                result.emplace_back(current);
                current.clear();
            } else {
                current.push_back(ch);
            }
        }
        result.push_back(current);
        return result;
    }
};


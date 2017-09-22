#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <iterator>

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

private:
    template<typename Out>
    static void split(const std::string &s, char delimiter, Out result, bool skip) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            if (!skip || item.length() > 0) *(result++) = item;
        }
    }

    static std::vector<std::string> split(const std::string &string, char delimiter, bool skip = true) {
        std::vector<std::string> strings;
        Command::split(string, delimiter, std::back_inserter(strings), skip);
        return strings;
    }
};


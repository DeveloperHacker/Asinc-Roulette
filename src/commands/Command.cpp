#include "Command.h"

Command::Command(const std::string &command) {
    auto &&vector = parse(command);
    name = vector[0];
    auto &&begin = vector.begin() + 1;
    auto &&end = vector.end();
    arguments = std::vector<std::string>(begin, end);
}

std::__cxx11::basic_string<char> Command::get_name() const {
    return name;
}

std::vector<std::string> Command::get_arguments() const {
    return arguments;
}

std::__cxx11::basic_string<char> Command::get_argument(size_t index) const {
    return arguments[index];
}

std::vector<std::string> Command::parse(const std::string &command) {
    std::vector<std::string> result;
    std::string current;
    bool in_string = false;
    char quote = ' ';
    for (auto ch : strings::trim_copy(command)) {
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

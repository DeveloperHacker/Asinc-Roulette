#pragma once

#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>

namespace strings {
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
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

    static std::string drop(const std::string &string, size_t offset) {
        auto &&length = string.size() - offset;
        std::string result(string.c_str() + offset, length);
        return result;
    }

    static bool str2int(int &integer, const std::string &string) {
        try {
            size_t position;
            integer = stoi(string, &position);
            return position == string.size();
        }
        catch (const std::invalid_argument &) {
            return false;
        }
    }
}
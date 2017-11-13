#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "../../commands/permitions.h"
#include <iostream>
#include <bitset>

template<typename R, typename... Args>
class Handlers {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

private:
    std::unordered_map<std::string, std::pair<permition_t, std::function<R(Args...)>>> handlers;

public:
    Handlers() = default;

    void add_handler(permition_t permition, const std::string &name, std::function<R(Args...)> &impl) {
        handlers.emplace(name, std::make_pair(permition, impl));
    }

    virtual R execute(permition_t permition, const std::string &name, Args... args) {
        auto &&entry = handlers.find(name);
        if (entry == std::end(handlers))
            throw error("handler with name " + name + " hasn't found");
        auto &&needed_permition = entry->second.first;
        std::cout << std::bitset<8>(permition) << " " << std::bitset<8>(needed_permition) << std::endl;
        if ((permition & needed_permition) == 0)
            throw error("execute permition denied");
        auto &&handler = entry->second.second;
        return handler(args...);
    }
};

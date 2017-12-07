#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "../commands/permissions.h"
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
    std::unordered_map<std::string, std::pair<permission_t, std::function<R(Args...)>>> handlers;

public:
    Handlers() = default;

    void add_handler(permission_t permission, const std::string &name, std::function<R(Args...)> &impl) {
        handlers.emplace(name, std::make_pair(permission, impl));
    }

    virtual R execute(permission_t permission, const std::string &name, Args... args) {
        auto &&entry = handlers.find(name);
        if (entry == std::end(handlers))
            throw error("handler with name " + name + " hasn't found");
        auto &&needed_permission = entry->second.first;
        if ((permission & needed_permission) == 0)
            throw error("execute permission denied");
        auto &&handler = entry->second.second;
        return handler(args...);
    }
};

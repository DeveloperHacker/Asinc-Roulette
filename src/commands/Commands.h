#pragma once

#include <unordered_map>
#include <functional>
#include "Command.h"
#include "permissions.h"

using args_t = std::vector<std::string>;
using impl_t = std::function<void(permission_t, const args_t &)>;

struct command_t {
    permission_t permission;
    std::string name;
    std::string argument_description;
    std::string description;
    impl_t impl;
};

class Commands {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

private:
    std::unordered_map<std::string, command_t> commands;

public:
    Commands() = default;

    void add_command(permission_t permission, const std::string &name, const std::string &argument_description,
                     const std::string &description, impl_t &impl);

    void parse_and_execute(permission_t permission, const std::string &raw_command);

    void execute(permission_t permission, const Command &command);

    std::unordered_map<std::string, std::tuple<std::string, std::string>> get_commands_info(permission_t permission);
};

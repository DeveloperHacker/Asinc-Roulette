#pragma once

#include <unordered_map>
#include "Command.h"
#include "permitions.h"
#include "../tcp/TCPServer.h"

using args_t = std::vector<std::string>;
using impl_t = std::function<void(permition_t, const args_t &)>;

struct command_t {
    permition_t permition;
    std::string name;
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

    void add_command(permition_t permition, const std::string &name, const std::string &description, impl_t &impl);

    void parse_and_execute(permition_t permition, const std::string &raw_command);

    void execute(permition_t permition, const Command &command);

    std::unordered_map<std::string, std::string> get_commands_info(permition_t permition);
};

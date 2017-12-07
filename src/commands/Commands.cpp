#include "Commands.h"
#include <bitset>

void
Commands::add_command(
        permission_t permission,
        const std::string &name,
        const std::string &argument_description,
        const std::string &description, impl_t &impl) {
    commands.emplace(name, command_t{permission, name, argument_description, description, impl});
}

void Commands::parse_and_execute(permission_t permission, const std::string &raw_command) {
    Command command(raw_command);
    execute(permission, command);
}

void Commands::execute(permission_t permission, const Command &command) {
    auto name = command.get_name();
    auto &&inst = commands.find(name);
    if (name.empty()) return;
    if (inst == std::end(commands))
        throw error("command " + name + " hasn't recognized");
    auto &&needed_permission = inst->second.permission;
    if (!(permission & needed_permission))
        throw error("execute permission denied");
    auto &&impl = inst->second.impl;
    impl(permission, command.get_arguments());
}

std::unordered_map<std::string, std::tuple<std::string, std::string>>
Commands::get_commands_info(permission_t permission) {
    std::unordered_map<std::string, std::tuple<std::string, std::string>> view;
    for (auto &&entry: commands) {
        auto &&needed_permission = entry.second.permission;
        auto &&name = entry.second.name;
        auto &&arguments = entry.second.argument_description;
        auto &&description = entry.second.description;
        if (permission & needed_permission) {
            view[name] = std::make_tuple(arguments, description);
        }
    }
    return view;
}

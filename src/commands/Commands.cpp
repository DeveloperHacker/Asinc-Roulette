#include "Commands.h"
#include <bitset>

void
Commands::add_command(
        permition_t permition,
        const std::string &name,
        const std::string &argument_description,
        const std::string &description, impl_t &impl) {
    commands.emplace(name, command_t{permition, name, argument_description, description, impl});
}

void Commands::parse_and_execute(permition_t permition, const std::string &raw_command) {
    Command command(raw_command);
    execute(permition, command);
}

void Commands::execute(permition_t permition, const Command &command) {
    auto name = command.get_name();
    auto &&inst = commands.find(name);
    if (name.empty()) return;
    if (inst == std::end(commands))
        throw error("command " + name + " hasn't recognized");
    auto &&needed_permition = inst->second.permition;
    if (!(permition & needed_permition))
        throw error("execute permition denied");
    auto &&impl = inst->second.impl;
    impl(permition, command.get_arguments());
}

std::unordered_map<std::string, std::tuple<std::string, std::string>>
Commands::get_commands_info(permition_t permition) {
    std::unordered_map<std::string, std::tuple<std::string, std::string>> view;
    for (auto &&entry: commands) {
        auto &&needed_permition = entry.second.permition;
        auto &&name = entry.second.name;
        auto &&arguments = entry.second.argument_description;
        auto &&description = entry.second.description;
        if (permition & needed_permition) {
            view[name] = std::make_tuple(arguments, description);
        }
    }
    return view;
}

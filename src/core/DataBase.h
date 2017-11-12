#pragma once

#include "../../commands/permitions.h"
#include <string>
#include <bits/shared_ptr.h>
#include <SQLiteCpp/SQLiteCpp.h>

class DataBase {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

private:
    std::shared_ptr<SQLite::Database> database;

public:
    explicit DataBase(const std::string &name);

    permition_t get_permition(const std::string &login);

    permition_t get_permition(const std::string &login, const std::string &password);

    void add_permition(const std::string &login, const std::string &password, permition_t permition);

    void set_permition(const std::string &login, permition_t permition);
};

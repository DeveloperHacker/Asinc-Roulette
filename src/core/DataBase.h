#pragma once

#include "../commands/permitions.h"
#include <string>
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>
#include <unordered_set>

using money_t = int;

class DataBase {
public:
    class error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        ~error() override = default;
    };

    struct user_t {
        std::string login;
        std::string password;
        permition_t permition;
        money_t balance;
    };

private:
    std::shared_ptr<SQLite::Database> database;

public:
    explicit DataBase(const std::string &name);

    user_t get_user(const std::string &login);

    void new_user(const std::string &login, const std::string &password, permition_t permition);

    permition_t get_user_permition(const std::string &login);

    money_t get_user_pouch(const std::string &login);

    void set_user_permition(const std::string &login, permition_t permition);

    void set_user_balance(const std::string &login, money_t pouch);

    static void init(const std::string &name);
};

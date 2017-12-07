#include <sstream>
#include <iostream>
#include "DataBase.h"
#include "config.h"


DataBase::DataBase(const std::string &name
) : database(std::make_shared<SQLite::Database>(name, SQLite::OPEN_READWRITE)) {}


DataBase::user_t DataBase::get_user(const std::string &login) {
    try {
        SQLite::Statement query(*database, "SELECT password, permission, balance FROM users WHERE login = ?");
        query.bind(1, login);
        auto &&status = query.executeStep();
        if (!status) throw DataBase::error("user hasn't found");
        std::string password = query.getColumn(0);
        permission_t permission = query.getColumn(1);
        money_t pouch = query.getColumn(2);
        return DataBase::user_t{login, password, permission, pouch};
    } catch (SQLite::Exception &ex) {
        throw DataBase::error("user hasn't found");
    }
}

void DataBase::new_user(const std::string &login, const std::string &password, permission_t permission) {
    SQLite::Statement query(*database, "INSERT INTO users VALUES (?, ?, ?, ?)");
    query.bind(1, login);
    query.bind(2, password);
    query.bind(3, permission);
    query.bind(4, other::START_UP_CAPITAL);
    try {
        SQLite::Transaction transaction(*database);
        auto &&status = query.exec();
        transaction.commit();
    } catch (SQLite::Exception &ex) {
        throw DataBase::error("user with login " + login + " already exist");
    }
}

void DataBase::set_user_permission(const std::string &login, permission_t permission) {
    SQLite::Statement query(*database, "UPDATE users SET permission = ? WHERE login = ?");
    query.bind(1, permission);
    query.bind(2, login);
    try {
        SQLite::Transaction transaction(*database);
        auto &&status = query.exec();
        transaction.commit();
    } catch (std::exception &ex) {
        throw DataBase::error("user with login " + login + " hasn't found");
    }
}

void DataBase::set_user_balance(const std::string &login, money_t pouch) {
    SQLite::Statement query(*database, "UPDATE users SET balance = ? WHERE login = ?");
    query.bind(1, pouch);
    query.bind(2, login);
    try {
        SQLite::Transaction transaction(*database);
        auto &&status = query.exec();
        transaction.commit();
    } catch (std::exception &ex) {
        throw DataBase::error("user with login " + login + " hasn't found");
    }
}

permission_t DataBase::get_user_permission(const std::string &login) {
    auto &&user = get_user(login);
    return user.permission;
}

money_t DataBase::get_user_pouch(const std::string &login) {
    auto &&user = get_user(login);
    return user.balance;
}

void DataBase::init(const std::string &name) {
    SQLite::Database db(name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec("DROP TABLE IF EXISTS users");
    SQLite::Transaction transaction(db);
    db.exec("CREATE TABLE users ("
                    "login TEXT NOT NULL PRIMARY KEY, "
                    "password TEXT NOT NULL, "
                    "permission INT NOT NULL,"
                    "balance INT NOT NULL"
                    ")");
    auto &&admin_login = other::ADMIN;
    auto &&admin_password = other::ADMIN;
    auto &&permission = std::to_string(permissions::ADMIN | permissions::STAFF);
    auto &&start_up_capital = std::to_string(other::START_UP_CAPITAL);
    auto &&guest_permission = std::to_string(permissions::GUEST);
    auto &&guest_login = other::GUEST;
    auto &&guest_password = other::GUEST;
    db.exec("INSERT INTO users VALUES ('admin', 'admin', " + permission + ", 10000000)");
    db.exec("INSERT INTO users VALUES ("
                    "'" + guest_login + "', "
                    "'" + guest_password + "', "
                    "" + guest_permission + ", "
                    "" + start_up_capital + ")");
    transaction.commit();
}

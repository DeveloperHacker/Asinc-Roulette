#include <sstream>
#include "DataBase.h"


DataBase::DataBase(const std::string &name
) : database(std::make_shared<SQLite::Database>(name, SQLite::OPEN_READWRITE)) {}

permition_t DataBase::get_permition(const std::string &login) {
    try {
        SQLite::Statement query(*database, "SELECT permition FROM users WHERE login = ?");
        query.bind(1, login);
        auto &&status = query.executeStep();
        if (status != 1)
            throw DataBase::error("invalid login");
        permition_t permition = query.getColumn(0);
        return permition;
    } catch (SQLite::Exception &ex) {
        throw DataBase::error("invalid login");
    }
}

permition_t DataBase::get_permition(const std::string &login, const std::string &password) {
    try {
        SQLite::Statement query(*database, "SELECT permition FROM users WHERE login = ? AND password = ?");
        query.bind(1, login);
        query.bind(2, password);
        auto &&status = query.executeStep();
        if (status != 1)
            throw DataBase::error("invalid login or password");
        permition_t permition = query.getColumn(0);
        return permition;
    } catch (SQLite::Exception &ex) {
        throw DataBase::error("invalid login or password");
    }
}

void DataBase::add_permition(const std::string &login, const std::string &password, permition_t permition) {
    SQLite::Statement query(*database, "INSERT INTO users VALUES (?, ?, ?)");
    query.bind(1, login);
    query.bind(2, password);
    query.bind(3, permition);
    try {
        SQLite::Transaction transaction(*database);
        auto &&status = query.exec();
        transaction.commit();
    } catch (SQLite::Exception &ex) {
        throw DataBase::error("user with login " + login + " already exist");
    }
}

void DataBase::set_permition(const std::string &login, permition_t permition) {
    SQLite::Statement query(*database, "UPDATE users SET permition = ? WHERE login = ?");
    query.bind(1, permition);
    query.bind(2, login);
    try {
        SQLite::Transaction transaction(*database);
        auto &&status = query.exec();
        transaction.commit();
    } catch (std::exception &ex) {
        throw DataBase::error("user with login " + login + " hasn't found");
    }
}

void DataBase::init(const std::string &name) {
    SQLite::Database db(name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec("DROP TABLE IF EXISTS users");
    SQLite::Transaction transaction(db);
    db.exec("CREATE TABLE users (login TEXT NOT NULL PRIMARY KEY, password TEXT NOT NULL, permition INT NOT NULL)");
    auto &&permition = permitions::ADMIN | permitions::STAFF;
    db.exec("INSERT INTO users VALUES ('admin', 'admin', " + std::to_string(permition) + ")");
    db.exec("INSERT INTO users VALUES ('guest', 'guest', " + std::to_string(permitions::NONE) + ")");
    transaction.commit();
}

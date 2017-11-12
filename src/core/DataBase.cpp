#include <sstream>
#include "DataBase.h"

std::string escape(const std::string &string) {
    return string;
}

DataBase::DataBase(const std::string &name
) : database(std::make_shared<SQLite::Database>(name, SQLite::OPEN_READWRITE)) {}

permition_t DataBase::get_permition(const std::string &login) {
    return 0;
}

permition_t DataBase::get_permition(const std::string &login, const std::string &password) {
    return 0;
}

void DataBase::add_permition(const std::string &login, const std::string &password, permition_t permition) {
    std::stringstream query;
    query << "INSERT INTO users "
          << "VALUES(" << "\"" << escape(login) << "\","
          << "\"" << escape(password) << "\","
          << permition << ")";
    try {
        SQLite::Transaction transaction(*database);
        database->exec(query.str());
        transaction.commit();
    } catch (std::exception &ex) {
        throw DataBase::error("invalid login or password");
    }
}

void DataBase::set_permition(const std::string &login, permition_t permition) {
    std::stringstream query;
    query << "UPDATE users "
          << "SET permition = \"" << permition << "\" "
          << "WHERE login = \"" << escape(login) << "\"";
    try {
        SQLite::Transaction transaction(*database);
        database->exec(query.str());
        transaction.commit();
    } catch (std::exception &ex) {
        throw DataBase::error("invalid login");
    }
}

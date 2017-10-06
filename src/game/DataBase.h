#pragma once

#include "Role.h"
#include <string>

class DataBase {
public:
    static const ID INVALID_ID = -1;

public:
    ID get_id(const std::string &login, const std::string &password);

    Role get_role(ID id);

    ID register_user(const std::string &login, const std::string &password);

    bool set_role(ID id, Role role);

    bool set_role(const std::string &login, Role role);
};

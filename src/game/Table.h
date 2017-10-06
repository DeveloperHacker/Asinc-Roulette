#pragma once

#include <vector>
#include "Role.h"

class Table {
private:
    ID croupier;

    std::vector<ID> gamers;

    size_t expect_gamers;


public:
    Table(ID croupier, size_t expect_gamers);

    bool add_user(ID user);

    bool remove_user(ID user);
};

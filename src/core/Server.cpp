#include "Server.h"
#include "config.h"

using json = nlohmann::json;

Server::Server(std::shared_ptr<Socket> socket) : SessionServer(socket),
                                                 handlers(std::make_shared<ServerHandlers>()),
                                                 data_base(other::DATA_BASE) {
    std::srand(std::time(nullptr));
}

void Server::handle_error(identifier_t id, const std::string &command, const std::string &message) {
    std::cerr << "[DEBUG] ERROR " << message << std::endl;
    send(id, stats::STATUS_ERROR, command, {
            {parts::MESSAGE, message}
    });
}

bool Server::session_handle(identifier_t id, const std::string &message) {
    auto &&request = json::parse(message);
    std::string command = request[parts::COMMAND];
    auto &&data = request[parts::DATA];
    try {
        auto &&permission = get_permission(id);
        return handlers->execute(permission, command, *this, id, data);
    } catch (ServerHandlers::error &ex) {
        handle_error(id, command, ex.what());
        return false;
    } catch (Server::error &ex) {
        handle_error(id, command, ex.what());
        return false;
    } catch (DataBase::error &ex) {
        handle_error(id, command, ex.what());
        return false;
    }
}

void Server::connect_handle(identifier_t id) {
    SessionServer::connect_handle(id);
    auto &&user = data_base.get_user(other::GUEST);
    add_user(id, user);
}

void Server::disconnect_handle(identifier_t id) {
    SessionServer::disconnect_handle(id);
    do_disconnect(id);
}

void Server::do_signin(identifier_t id, const std::string &login, const std::string &password) {
    if (login2id.count(login) != 0)
        throw Server::error("user with login " + login + " already logged in");
    auto &&user = data_base.get_user(login);
    if (user.password != password)
        throw Server::error("invalid login or password");
    add_user(id, user);
    send(id, stats::STATUS_SUCCESS, commands::SIGNIN, {
            {parts::PERMITION, user.permission}
    });
}

void Server::do_signout(identifier_t id) {
    auto &&user = data_base.get_user(other::GUEST);
    add_user(id, user);
    send(id, stats::STATUS_SUCCESS, commands::SINGOUT, {
            {parts::PERMITION, permissions::GUEST}
    });
}

void Server::do_join(identifier_t id, const std::string &name, const std::string &password) {
    add_player(id, name, password);
    send(id, stats::STATUS_SUCCESS, commands::JOIN, {
            {parts::PERMITION, permissions::PLAYER}
    });
}

void Server::do_create(identifier_t id, const std::string &name, const std::string &password) {
    add_croupier(id, name, password);
    send(id, stats::STATUS_SUCCESS, commands::CREATE, {
            {parts::PERMITION, permissions::CROUPIER}
    });
}

void Server::do_leave(identifier_t id) {
    auto &&permission = get_permission(id);
    if (permission & permissions::CROUPIER)
        leave_croupier(id);
    if (permission & permissions::PLAYER)
        leave_player(id);
}

void Server::do_write(identifier_t id, const std::string &message) {
    auto &&session = get_session(id);
    for (auto &&login : session.users) {
        auto &&dest_id = get_id(login);
        if (id == dest_id) continue;
        do_write(id, login, message);
    }
}

void Server::do_write(identifier_t id, const std::string &login, const std::string &message) {
    auto &&session = get_session(id);
    if (session.users.count(login) == 0)
        throw Server::error("user with login " + login + " hasn't found");
    send(get_id(login), stats::STATUS_SUCCESS, commands::WRITE, {
            {parts::LOGIN,   get_login(id)},
            {parts::MESSAGE, message}
    });
}

void Server::do_tables(identifier_t id) {
    json list = {};
    for (auto &&entry : sessions) {
        auto &&session = entry.second;
        json table;
        table[parts::NAME] = session.name;
        table[parts::NUM_PLAYERS] = session.users.size();
        table[parts::MAX_PLAYERS] = session.max_players;
        table[parts::LOCK] = !session.password.empty();
        list.push_back(table);
    }
    send(id, stats::STATUS_SUCCESS, commands::TABLES, {
            {parts::LIST, list}
    });
}

void Server::do_users(identifier_t id) {
    auto &&session = get_session(id);
    json list = {};
    for (auto &&login : session.users) {
        auto &&user_id = get_id(login);
        auto &&permission = get_permission(user_id);
        auto &&role = permission & permissions::CROUPIER ? other::CROUPIER : other::PLAYER;
        json user;
        user[parts::LOGIN] = login;
        user[parts::ROLE] = role;
        list.push_back(user);
    }
    send(id, stats::STATUS_SUCCESS, commands::USERS, {
            {parts::LIST, list}
    });
}

void Server::do_disconnect(identifier_t id) {
    auto &&login = get_login(id);
    auto &&permission = get_permission(id);
    if (permission & permissions::CROUPIER)
        leave_croupier(id);
    if (permission & permissions::PLAYER)
        leave_player(id);
    remove_user(id);
}

void Server::do_sync(identifier_t id) {
    auto &&permission = get_permission(id);
    send(id, stats::STATUS_SUCCESS, commands::SYNC, {
            {parts::PERMITION, permission}
    });
}

void Server::do_signup(identifier_t id, const std::string &login, const std::string &password) {
    data_base.new_user(login, password, permissions::USER);
    send(id, stats::STATUS_SUCCESS, commands::SINGUP, {});
}

void Server::do_set_permission(identifier_t id, const std::string &login, permission_t permission) {
    data_base.set_user_permission(login, permission);
    auto &&user_online = login2id.count(login) > 0;
    if (user_online) {
        auto &&user_id = get_id(login);
        local_set_user_permission(user_id, permission);
        do_sync(user_id);
    }
    send(id, stats::STATUS_SUCCESS, commands::SET_PERMISSION, {});
}

std::string Server::get_login(identifier_t id) {
    auto &&user = get_user(id);
    return user.login;
}

permission_t Server::get_permission(identifier_t id) {
    auto &&user = get_user(id);
    return user.permission;
}

void Server::leave_croupier(identifier_t id) {
    auto &&session = get_session(id);
    for (auto &&login : std::unordered_set<std::string>(session.users)) {
        if (login == session.croupier) continue;
        leave_player(get_id(login));
    }
    sessions.erase(session.name);
    auto &&login = get_login(id);
    auto &&permission = data_base.get_user_permission(login);
    local_set_user_permission(id, permission);
    send(id, stats::STATUS_SUCCESS, commands::LEAVE, {
            {parts::PERMITION, permission}
    });
}

void Server::leave_player(identifier_t id) {
    auto &&session = get_session(id);
    auto &&login = get_login(id);
    if (session.croupier == login)
        throw Server::error("croupier is not player");
    session.users.erase(login);
    session.bets.erase(login);
    auto &&permission = data_base.get_user_permission(login);
    local_set_user_permission(id, permission);
    send(id, stats::STATUS_SUCCESS, commands::LEAVE, {
            {parts::PERMITION, permission}
    });
}

std::string Server::get_session_name(identifier_t id) {
    auto &&entry = id2name.find(id);
    if (entry == std::end(id2name))
        throw Server::error("session hasn't found");
    return entry->second;
}

Server::session_t &Server::get_session(identifier_t id) {
    std::string name = get_session_name(id);
    auto &&entry = sessions.find(name);
    if (entry == std::end(sessions))
        throw Server::error("session hasn't found");
    return entry->second;
}

void Server::add_player(identifier_t id, const std::string &name, const std::string &password) {
    auto &&entry = sessions.find(name);
    if (entry == std::end(sessions))
        throw Server::error("table with name " + name + " hasn't found");
    auto &&session = entry->second;
    if (session.max_players == session.users.size())
        throw Server::error("table is full");
    if (session.password != password)
        throw Server::error("incorrect password");
    auto &&login = get_login(id);
    id2name[id] = name;
    session.users.emplace(login);
    local_set_user_permission(id, permissions::PLAYER);
}

void Server::add_croupier(identifier_t id, const std::string &name, const std::string &password) {
    auto &&entry = sessions.find(name);
    if (entry != std::end(sessions))
        throw Server::error("table with name " + name + " already exist");
    auto &&login = get_login(id);
    session_t session{name, password, login, {}, other::MAX_PLAYERS};
    session.users.emplace(login);
    sessions[name] = session;
    id2name[id] = name;
    local_set_user_permission(id, permissions::CROUPIER);
}

void Server::add_user(identifier_t id, const DataBase::user_t &user) {
    auto &&login = user.login;
    login2id[login] = id;
    users[id] = user;
}

void Server::remove_user(identifier_t id) {
    auto &&login = get_login(id);
    login2id.erase(login);
    users.erase(id);
}

DataBase::user_t &Server::get_user(identifier_t id) {
    auto &&entry = users.find(id);
    if (entry == std::end(users))
        throw Server::error("connection hasn't found");
    return entry->second;
}

void Server::local_set_user_permission(identifier_t id, permission_t permission) {
    auto &&user = get_user(id);
    user.permission = permission;
    add_user(id, user);
}


identifier_t Server::get_id(const std::string &login) {
    auto &&entry = login2id.find(login);
    if (entry == std::end(login2id))
        throw Server::error("user with id hasn't found");
    return entry->second;
}

void Server::send(identifier_t id, const std::string &status, const std::string &command, const json &data) {
    json response;
    response[parts::STATUS] = status;
    response[parts::COMMAND] = command;
    response[parts::DATA] = data;
    send(id, response.dump());
}

void Server::do_balance(identifier_t id) {
    auto &&user = get_user(id);
    send(id, stats::STATUS_SUCCESS, commands::BALANCE, {
            {parts::BALANCE, user.balance}
    });
}

void Server::do_bet(identifier_t id, const bet_t &bet) {
    validate(bet);
    auto &&session = get_session(id);
    auto &&user = get_user(id);
    auto &&login = user.login;
    auto &&entry = session.bets.find(login);
    std::vector<bet_t> bets;
    if (entry != std::end(session.bets))
        bets = entry->second;
    int value = 0;
    for (auto &&_bet : bets)
        value += _bet.value;
    if (value + bet.value > user.balance)
        throw Server::error("not enough money");
    bets.push_back(bet);
    session.bets[login] = bets;
    send(id, stats::STATUS_SUCCESS, commands::BET, {});
}

void Server::do_bets(identifier_t id) {
    auto &&session = get_session(id);
    json data;
    for (auto &&entry : session.bets) {
        auto &&login = entry.first;
        for (auto &&bet : entry.second) {
            data.push_back({
                                   {parts::LOGIN,  login},
                                   {parts::TYPE,   bet.type},
                                   {parts::NUMBER, bet.number},
                                   {parts::VALUE,  bet.value}
                           });
        }
    }
    send(id, stats::STATUS_SUCCESS, commands::BETS, data);
}

void Server::do_spin(identifier_t id) {
    auto &&session = get_session(id);
    auto &&random_number = std::rand() % bets::MAX_NUMBER;
    for (auto &&login: session.users) {
        auto &&user_id = get_id(login);
        send(user_id, stats::STATUS_SUCCESS, commands::SPIN, {
                {parts::RANDOM_NUMBER, random_number}
        });
    }
    for (auto &&entry: session.bets) {
        auto &&login = entry.first;
        auto &&bets = entry.second;
        auto &&user_id = get_id(login);
        for (auto &&bet : bets) {
            auto &&is_positive = wining(random_number, bet);
            pay(user_id, bet, is_positive);
        }
    }
    session.bets.clear();
}

void Server::do_kick(identifier_t id, const std::string &login) {
    auto &&user_id = get_id(login);
    leave_player(user_id);
    send(id, stats::STATUS_SUCCESS, commands::KICK, {});
}

void Server::pay(identifier_t id, const bet_t &bet, bool is_positive) {
    money_t value;
    if (is_positive) {
        auto &&multiplier = bets::multipliers.find(bet.type)->second;
        value = bet.value * multiplier;
    } else {
        value = -bet.value;
    }
    auto &&user = get_user(id);
    user.balance += value;
    data_base.set_user_balance(user.login, user.balance);
    send(id, stats::STATUS_SUCCESS, commands::PAY, {
            {parts::TYPE,   bet.type},
            {parts::NUMBER, bet.number},
            {parts::VALUE,  value}
    });
}

void Server::validate(const bet_t &bet) {
    auto &&number = bet.number;
    auto &&bet_type = bet.type;
    auto &&entry = bets::ranges.find(bet_type);
    if (entry == std::end(bets::ranges))
        throw Server::error("illegal bet type " + bet_type);
    auto &&range = entry->second;
    auto &&norm_number = number + 1;
    auto &&cond = std::get<0>(range) <= norm_number && norm_number <= std::get<1>(range);
    if (!cond) throw Server::error("illegal bet number " + std::to_string(number));
}

bool Server::wining(int random_number, const bet_t &bet) {
    auto &&number = bet.number;
    auto &&bet_type = bet.type;
    bool is_zero = random_number == 0 || random_number == 37;
    bool is_red = bets::reds.count(random_number) != 0;
    if (bet_type == bets::RED) {
        return is_red;
    } else if (bet_type == bets::BLACK) {
        return !is_red && !is_zero;
    } else if (bet_type == bets::EVEN) {
        return !(random_number & 1);
    } else if (bet_type == bets::ODD) {
        return !!(random_number & 1);
    } else if (bet_type == bets::LOW) {
        return random_number < 19;
    } else if (bet_type == bets::HIGH) {
        return random_number > 18;
    } else if (bet_type == bets::ZERO) {
        return is_zero;
    } else if (bet_type == bets::BASKET) {
        return random_number >= 0 && random_number <= 3 || random_number == 37;
    } else if (bet_type == bets::COLUMN) {
        return random_number % 12 == number && !is_zero;
    } else if (bet_type == bets::DOZEN) {
        return random_number % 3 == number && !is_zero;
    } else if (bet_type == bets::LINE) {
        return random_number % 6 == number && !is_zero;
    } else if (bet_type == bets::STREET) {
        return random_number % 3 == number && !is_zero;
    } else if (bet_type == bets::CORNER) {
        auto &&numbers = bets::corners[number];
        return numbers.count(random_number) != 0;
    } else if (bet_type == bets::HSPLIT) {
        if (is_zero) return false;
        auto &&line = random_number / 3 * 2;
        auto &&position = random_number % 3;
        if (line != number && line + 1 != number) return false;
        if (number % 2 == 0 && (position == 0 || position == 1)) return true;
        return number % 2 == 1 && (position == 1 || position == 2);
    } else if (bet_type == bets::VSPLIT) {
        return (random_number == number || random_number - 3 == number) && !is_zero;
    } else if (bet_type == bets::STRAIGHT) {
        return number == random_number;
    } else {
        throw Server::error("undefined bet type " + bet_type);
    }
}

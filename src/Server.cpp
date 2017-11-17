#include "Server.h"
#include "config.h"
#include <ctime>

using json = nlohmann::json;

Server::Server(int domain, int type, int protocol, address_t &address
) : CryptoServer(domain, type, protocol, address),
    handlers(std::make_shared<ServerHandlers>()),
    data_base(other::DATA_BASE) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Server::handle_error(id_t id, const std::string &command, const std::string &message) {
    send(id, stats::ERROR, command, {
            {parts::MESSAGE, message}
    });
}

bool Server::crypto_handle(id_t id, address_t address, const std::string &message) {
    auto &&request = json::parse(message);
    std::cout << request.dump(4) << std::endl;
    std::string command = request[parts::COMMAND];
    auto &&data = request[parts::DATA];
    try {
        auto &&permition = get_permition(id);
        return handlers->execute(permition, command, *this, id, address, data);
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

void Server::connect_handle(id_t id, address_t address) {
    CryptoServer::connect_handle(id, address);
    auto &&user = data_base.get_user(other::GUEST);
    add_user(id, user);
}

void Server::disconnect_handle(id_t id, address_t address) {
    CryptoServer::disconnect_handle(id, address);
    disconnect(id);
}

void Server::login(id_t id, const std::string &login, const std::string &password) {
    if (login2id.count(login) != 0)
        throw Server::error("user with login " + login + " already logged in");
    auto &&user = data_base.get_user(login);
    if (user.password != password)
        throw Server::error("invalid login or password");
    add_user(id, user);
    send(id, stats::RESOLVED, commands::SIGNUP, {
            {parts::PERMITION, user.permition}
    });
}

void Server::logout(id_t id) {
    auto &&user = data_base.get_user(other::GUEST);
    add_user(id, user);
    send(id, stats::RESOLVED, commands::SINGOUT, {
            {parts::PERMITION, permitions::GUEST}
    });
}

void Server::join(id_t id, const std::string &name, const std::string &password) {
    add_player(id, name, password);
    send(id, stats::RESOLVED, commands::JOIN, {
            {parts::PERMITION, permitions::PLAYER}
    });
}

void Server::create(id_t id, const std::string &name, const std::string &password) {
    add_croupier(id, name, password);
    send(id, stats::RESOLVED, commands::CREATE, {
            {parts::PERMITION, permitions::CROUPIER}
    });
}

void Server::leave(id_t id) {
    auto &&permition = get_permition(id);
    if (permition & permitions::CROUPIER)
        leave_croupier(id);
    if (permition & permitions::PLAYER)
        leave_player(id);
}

void Server::write(id_t id, const std::string &message) {
    auto &&session = get_session(id);
    for (auto &&login : session.users) {
        auto &&dest_id = get_id(login);
        if (id == dest_id) continue;
        write(id, login, message);
    }
}

void Server::write(id_t id, const std::string &login, const std::string &message) {
    auto &&session = get_session(id);
    if (session.users.count(login) == 0)
        throw Server::error("user with login " + login + " hasn't found");
    send(get_id(login), stats::RESOLVED, commands::WRITE, {
            {parts::LOGIN,   get_login(id)},
            {parts::MESSAGE, message}
    });
}

void Server::send_tables(id_t id) {
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
    send(id, stats::RESOLVED, commands::TABLES, {
            {parts::LIST, list}
    });
}

void Server::send_users(id_t id) {
    auto &&session = get_session(id);
    json list = {};
    for (auto &&login : session.users) {
        auto &&user_id = get_id(login);
        auto &&permition = get_permition(user_id);
        auto &&role = permition & permitions::CROUPIER ? other::CROUPIER : other::PLAYER;
        json user;
        user[parts::LOGIN] = login;
        user[parts::ROLE] = role;
        list.push_back(user);
    }
    send(id, stats::RESOLVED, commands::USERS, {
            {parts::LIST, list}
    });
}

void Server::disconnect(id_t id) {
    auto &&login = get_login(id);
    auto &&permition = get_permition(id);
    try {
        if (permition & permitions::CROUPIER)
            leave_croupier(id);
        if (permition & permitions::PLAYER)
            leave_player(id);
    } catch (Server::error &ex) {
        // ignore
    }
    remove_user(id);
}

void Server::sync(id_t id) {
    auto &&permition = get_permition(id);
    send(id, stats::RESOLVED, commands::SYNC, {
            {parts::PERMITION, permition}
    });
}

void Server::registration(id_t id, const std::string &login, const std::string &password) {
    data_base.new_user(login, password, permitions::USER);
    send(id, stats::RESOLVED, commands::SIGNIN, {});
}

void Server::set_permition(id_t id, const std::string &login, permition_t permition) {
    data_base.set_user_permition(login, permition);
    auto &&user_online = login2id.count(login) > 0;
    if (user_online) {
        auto &&user_id = get_id(login);
        local_set_user_permition(user_id, permition);
        sync(user_id);
    }
    send(id, stats::RESOLVED, commands::SET_PERMITION, {});
}

std::string Server::get_login(id_t id) {
    auto &&user = get_user(id);
    return user.login;
}

permition_t Server::get_permition(id_t id) {
    auto &&user = get_user(id);
    return user.permition;
}

void Server::leave_croupier(id_t id) {
    auto &&session = get_session(id);
    for (auto &&login : session.users) {
        if (login == session.croupier) continue;
        leave_player(get_id(login));
    }
    sessions.erase(session.name);
    auto &&login = get_login(id);
    auto &&permition = data_base.get_user_permition(login);
    local_set_user_permition(id, permition);
    send(id, stats::RESOLVED, commands::LEAVE, {
            {parts::PERMITION, permition}
    });
}

void Server::leave_player(id_t id) {
    auto &&session = get_session(id);
    auto &&login = get_login(id);
    session.users.erase(login);
    auto &&permition = data_base.get_user_permition(login);
    local_set_user_permition(id, permition);
    send(id, stats::RESOLVED, commands::LEAVE, {
            {parts::PERMITION, permition}
    });
}

std::string Server::get_session_name(id_t id) {
    auto &&entry = id2name.find(id);
    if (entry == std::end(id2name))
        throw Server::error("session hasn't found");
    return entry->second;
}

Server::session_t Server::get_session(id_t id) {
    std::string name = get_session_name(id);
    auto &&entry = sessions.find(name);
    if (entry == std::end(sessions))
        throw Server::error("session hasn't found");
    return entry->second;
}

void Server::add_player(id_t id, const std::string &name, const std::string &password) {
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
    local_set_user_permition(id, permitions::PLAYER);
}

void Server::add_croupier(id_t id, const std::string &name, const std::string &password) {
    auto &&entry = sessions.find(name);
    if (entry != std::end(sessions))
        throw Server::error("table with name " + name + " already exist");
    auto &&login = get_login(id);
    session_t session{name, password, login, {}, other::MAX_PLAYERS};
    session.users.emplace(login);
    sessions[name] = session;
    id2name[id] = name;
    local_set_user_permition(id, permitions::CROUPIER);
}

void Server::add_user(id_t id, const DataBase::user_t &user) {
    auto &&login = user.login;
    login2id[login] = id;
    users[id] = user;
}

void Server::remove_user(id_t id) {
    auto &&login = get_login(id);
    login2id.erase(login);
    users.erase(id);
}

DataBase::user_t Server::get_user(id_t id) {
    auto &&entry = users.find(id);
    if (entry == std::end(users))
        throw Server::error("connection hasn't found");
    return entry->second;
}

void Server::local_set_user_permition(id_t id, permition_t permition) {
    auto &&user = get_user(id);
    user.permition = permition;
    add_user(id, user);
}


id_t Server::get_id(const std::string &login) {
    auto &&entry = login2id.find(login);
    if (entry == std::end(login2id))
        throw Server::error("user with id hasn't found");
    return entry->second;
}

void Server::send(id_t id, const std::string &status, const std::string &command, const json &data) {
    json response;
    response[parts::STATUS] = status;
    response[parts::COMMAND] = command;
    response[parts::DATA] = data;
    std::cout << response.dump(4) << std::endl;
    send(id, response.dump());
}

void Server::balance(id_t id) {
    auto &&user = get_user(id);
    send(id, stats::RESOLVED, commands::BALANCE, {
            {parts::BALANCE, user.balance}
    });
}

void Server::bet(id_t id, const bet_t &bet) {
    validate(bet);
    auto &&session = get_session(id);
    auto &&user = get_user(id);
    auto &&login = user.login;
    auto &&entry = session.bets.find(login);
    std::vector<bet_t> bets;
    if (entry != std::end(session.bets))
        bets = entry->second;
    int value = 0;
    for (auto &&bet : bets)
        value += bet.value;
    if (value > user.balance)
        throw Server::error("not enough money");
    send(id, stats::RESOLVED, commands::BET, {});
}

void Server::bets(id_t id) {
    auto &&session = get_session(id);
    json data;
    for (auto &&entry : session.bets) {
        auto &&login = entry.first;
        auto &&bets = entry.second;
        for (auto &&bet : bets) {
            json js_bet = {
                    {parts::BET_TYPE,   bet.type},
                    {parts::BET_NUMBER, bet.number},
                    {parts::BET_VALUE,  bet.value}
            };
            data[login].push_back(js_bet);
        }
    }
    send(id, stats::RESOLVED, commands::BETS, data);
}

void Server::spin(id_t id) {
    auto &&session = get_session(id);
    auto &&random_number = std::rand() % bets::MAX_NUMBER;
    for (auto &&login: session.users) {
        auto &&user_id = get_id(login);
        send(user_id, stats::RESOLVED, commands::SPIN, {
                {parts::RANDOM_NUMBER, random_number}
        });
    }
    for (auto &&entry: session.bets) {
        auto &&login = entry.first;
        auto &&bets = entry.second;
        auto &&user_id = get_id(login);
        for (auto &&bet : bets) {
            auto &&is_positive = wining(random_number, bet);
            pay(id, bet, is_positive);
        }
    }
    session.bets.clear();
}

void Server::pay(id_t id, const bet_t &bet, bool is_positive) {
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
    send(id, stats::RESOLVED, commands::PAY, {
            {parts::BET_TYPE,   bet.type},
            {parts::BET_NUMBER, bet.number},
            {parts::BET_VALUE,  value}
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
    if (!cond) throw Server::error("illegal bet number " + number);
}

bool Server::wining(int random_number, const bet_t &bet) {
    auto &&number = bet.number;
    auto &&bet_type = bet.type;
    bool is_zero = random_number == 0 || random_number == 37;
    bool is_red = bets::reds.count(number) > 0;
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

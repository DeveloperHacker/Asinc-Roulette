#include "Server.h"
#include "config.h"
#include "../lib/json/src/json.hpp"

using json = nlohmann::json;

Server::Server(int domain, int type, int protocol, address_t &address
) : CryptoServer(domain, type, protocol, address),
    handlers(std::make_shared<ServerHandlers>()),
    data_base(other::DATA_BASE) {
}

void Server::handle_error(id_t id, const std::string &command, const std::string &message) {
    json response;
    response[parts::STATUS] = stats::ERROR;
    response[parts::COMMAND] = command;
    response[parts::MESSAGE] = message;
    send(id, response.dump());
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
    add_auth(id, other::GUEST, permitions::GUEST);
}

void Server::disconnect_handle(id_t id, address_t address) {
    CryptoServer::disconnect_handle(id, address);
    disconnect(id);
}

void Server::login(id_t id, const std::string &login, const std::string &password) {
    auto &&permition = data_base.get_permition(login, password);
    std::cout << std::bitset<8>(permition) << std::endl;
    add_auth(id, login, permition);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::LOGIN;
    response[parts::DATA][parts::PERMITION] = permition;
    send(id, response.dump());
}

void Server::logout(id_t id) {
    auto &&login = get_login(id);
    add_auth(id, login, permitions::GUEST);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::LOGOUT;
    response[parts::DATA][parts::PERMITION] = permitions::GUEST;
    send(id, response.dump());
}

void Server::join(id_t id, const std::string &name, const std::string &password) {
    add_player(id, name, password);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::JOIN;
    response[parts::DATA][parts::PERMITION] = permitions::PLAYER;
    send(id, response.dump());
}

void Server::create(id_t id, const std::string &name, const std::string &password) {
    add_croupier(id, name, password);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::CREATE;
    response[parts::DATA][parts::PERMITION] = permitions::CROUPIER;
    send(id, response.dump());
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
    for (auto &&login : session.players) {
        auto &&dest_id = get_id(login);
        if (id != dest_id)
            write(id, login, message);
    }
}

void Server::write(id_t id, const std::string &login, const std::string &message) {
    auto &&session = get_session(id);
    if (session.players.count(login) == 0)
        throw Server::error("user with login " + login + " hasn't found");
    auto &&dest_id = get_id(login);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::WRITE;
    response[parts::DATA][parts::MESSAGE] = message;
    send(id, response.dump());
}

void Server::tables(id_t id) {
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::TABLES;
    response[parts::DATA][parts::LIST] = {};
    for (auto &&entry : sessions) {
        auto &&session = entry.second;
        json table;
        table[parts::NAME] = session.name;
        table[parts::NUM_PLAYERS] = session.players.size();
        table[parts::MAX_PLAYERS] = session.max_players;
        table[parts::LOCK] = !session.password.empty();
        response[parts::DATA][parts::LIST].push_back(table);
    }
    send(id, response.dump());
}

void Server::users(id_t id) {
    auto &&session = get_session(id);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::USERS;
    response[parts::DATA][parts::LIST] = {};
    for (auto &&login : session.players) {
        auto &&dest_id = get_id(login);
        auto &&dest_permition = get_permition(dest_id);
        auto &&dest_role = dest_permition & permitions::CROUPIER ? other::CROUPIER : other::PLAYER;
        json user;
        user[parts::LOGIN] = login;
        user[parts::ROLE] = dest_role;
        response[parts::DATA][parts::LIST].push_back(user);
    }
    send(id, response.dump());
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
    ids.erase(login);
    auth.erase(id);
}

void Server::sync(id_t id) {
    auto &&permition = get_permition(id);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::SYNC;
    response[parts::DATA][parts::PERMITION] = permition;
    send(id, response.dump());
}

void Server::registration(id_t id, const std::string &login, const std::string &password) {
    data_base.add_permition(login, password, permitions::USER);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::REGISTRATION;
    send(id, response.dump());
}

void Server::set_permition(id_t id, const std::string &login, permition_t permition) {
    std::cout << login << std::endl;
    data_base.set_permition(login, permition);
    auto &&user_online = ids.count(login) > 0;
    if (user_online) {
        auto &&user_id = get_id(login);
        add_auth(user_id, login, permition);
        sync(user_id);
    }
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::SET_PERMITION;
    send(id, response.dump());
}

std::string Server::get_login(id_t id) {
    auto &&entry = auth.find(id);
    if (entry == std::end(auth))
        throw Server::error("connection hasn't found");
    auto &&login = std::get<0>(entry->second);
    return login;
}

permition_t Server::get_permition(id_t id) {
    auto &&entry = auth.find(id);
    if (entry == std::end(auth))
        throw Server::error("connection hasn't found");
    auto &&permition = std::get<1>(entry->second);
    std::cout << std::get<0>(entry->second) << " " << permition << std::endl;
    return permition;
}

void Server::leave_croupier(id_t id) {
    auto &&session = get_session(id);
    for (auto &&player : session.players) {
        auto &&dest_id = get_id(player);
        leave_player(dest_id);
    }
    sessions.erase(session.name);
    auto &&login = get_login(id);
    auto &&permition = data_base.get_permition(login);
    add_auth(id, login, permition);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::LEAVE;
    response[parts::DATA][parts::PERMITION] = permition;
    send(id, response.dump());
}

void Server::leave_player(id_t id) {
    auto &&session = get_session(id);
    auto &&login = get_login(id);
    session.players.erase(login);
    auto &&permition = data_base.get_permition(login);
    add_auth(id, login, permition);
    json response;
    response[parts::STATUS] = stats::RESOLVED;
    response[parts::COMMAND] = commands::LEAVE;
    response[parts::DATA][parts::PERMITION] = permition;
    send(id, response.dump());
}

std::string Server::get_session_name(id_t id) {
    auto &&permition = get_permition(id);
    auto &&login = get_login(id);
    if (permition & permitions::CROUPIER) {
        auto &&entry = croupiers.find(login);
        if (entry == std::end(croupiers))
            throw Server::error("player hasn't found");
        return entry->second;
    } else if (permition & permitions::PLAYER) {
        auto &&entry = players.find(login);
        if (entry == std::end(players))
            throw Server::error("player hasn't found");
        return entry->second;
    } else {
        throw Server::error("session hasn't found");
    }
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
    if (session.max_players == session.players.size())
        throw Server::error("table is full");
    if (session.password != password)
        throw Server::error("incorrect password");
    auto &&login = get_login(id);
    players[login] = name;
    session.players.emplace(login);
    add_auth(id, login, permitions::PLAYER);
}

void Server::add_croupier(id_t id, const std::string &name, const std::string &password) {
    auto &&entry = sessions.find(name);
    if (entry != std::end(sessions))
        throw Server::error("table with name " + name + " already exist");
    auto &&login = get_login(id);
    croupiers[login] = name;
    sessions[name] = session_t{name, password, login, {}, other::MAX_PLAYERS};
    add_auth(id, login, permitions::CROUPIER);
}

void Server::add_auth(id_t id, const std::string &login, permition_t permition) {
    std::cout << id << " " << login << " " << std::bitset<8>(permition) << std::endl;
    ids[login] = id;
    auth[id] = std::make_tuple(login, permition);
}

id_t Server::get_id(const std::string &login) {
    auto &&entry = ids.find(login);
    if (entry == std::end(ids))
        throw Server::error("user with id hasn't found");
    return entry->second;
}

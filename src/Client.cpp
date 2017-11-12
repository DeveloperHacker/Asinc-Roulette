#include "Client.h"
#include "../lib/json/src/json.hpp"
#include "config.h"

using json = nlohmann::json;

Client::Client(int domain, int type, int protocol, address_t &address
) : CryptoClient(domain, type, protocol, address), permition(permitions::GUEST),
    handlers(std::make_shared<ClientHandlers>()),
    commands(std::make_shared<ClientCommands>()) {
    commands->init(*this);
}

void Client::crypto_input(const std::string &message) {
    try {
        auto &&response = json::parse(message);
        auto &&status = response[parts::STATUS];
        auto &&command = response[parts::COMMAND].dump();
        auto &&data = response[parts::DATA].dump();
        if (status == stats::ERROR)
            throw Client::error("Error " + command + ": " + data);
        handlers->execute(permition, command, *this, data);
    } catch (ClientHandlers::error &ex) {
        std::cerr << ex.what() << std::endl;
    } catch (Client::error &ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Client::crypto_output() {
    try {
        std::string command;
        std::getline(std::cin, command);
        commands->parse_and_execute(permition, command);
    } catch (ClientCommands::error &ex) {
        std::cerr << ex.what() << std::endl;
    } catch (Client::error &ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Client::login(const std::string &login, const std::string &password) {
    json request;
    request[parts::COMMAND] = commands::LOGIN;
    request[parts::DATA][parts::LOGIN] = login;
    request[parts::DATA][parts::PASSWORD] = password;
    send(request.dump());
}

void Client::logout() {
    json request;
    request[parts::COMMAND] = commands::LOGOUT;
    send(request.dump());
}

void Client::join(const std::string &name, const std::string &password) {
    json request;
    request[parts::COMMAND] = commands::JOIN;
    request[parts::DATA][parts::NAME] = name;
    request[parts::DATA][parts::PASSWORD] = password;
    send(request.dump());
}

void Client::create(const std::string &name, const std::string &password) {
    json request;
    request[parts::COMMAND] = commands::CREATE;
    request[parts::DATA][parts::NAME] = name;
    request[parts::DATA][parts::PASSWORD] = password;
    send(request.dump());
}

void Client::leave() {
    json request;
    request[parts::COMMAND] = commands::LEAVE;
    send(request.dump());
}

void Client::write(const std::string &message) {
    json request;
    request[parts::COMMAND] = commands::WRITE;
    request[parts::DATA][parts::MESSAGE] = message;
    send(request.dump());
}

void Client::tables() {
    json request;
    request[parts::COMMAND] = commands::TABLES;
    send(request.dump());
}

void Client::users() {
    json request;
    request[parts::COMMAND] = commands::USERS;
    send(request.dump());
}

void Client::disconnect() {
    json request;
    request[parts::COMMAND] = commands::DISCONNECT;
    send(request.dump());
    stop();
}

void Client::sync() {
    json request;
    request[parts::COMMAND] = commands::SYNC;
    send(request.dump());
}

void Client::registration(const std::string &login, const std::string &password) {
    json request;
    request[parts::COMMAND] = commands::REGISTRATION;
    request[parts::DATA][parts::LOGIN] = login;
    request[parts::DATA][parts::PASSWORD] = password;
    send(request.dump());
}

void Client::set_permition(const std::string &login, permition_t permition) {
    if (permition & !permitions::AUTH != 0)
        throw Client::error("changing permition on not AUTH permitions is impossible");
    json request;
    request[parts::COMMAND] = commands::SET_PERMITION;
    request[parts::DATA][parts::LOGIN] = login;
    request[parts::DATA][parts::PERMITION] = permition;
    send(request.dump());
}

void Client::join() {
    TCPClient::join();
}

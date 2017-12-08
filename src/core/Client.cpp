#include "Client.h"
#include "config.h"

using json = nlohmann::json;

Client::Client(std::shared_ptr<Socket> socket) : SessionClient(socket), permission(permissions::GUEST),
    handlers(std::make_shared<ClientHandlers>()),
    commands(std::make_shared<ClientCommands>()) {
    commands->init(*this);
}

void Client::session_input(const std::string &message) {
    try {
        auto &&response = json::parse(message);
        std::string status = response[parts::STATUS];
        std::string command = response[parts::COMMAND];
        auto &&data = response[parts::DATA];
        if (status == stats::STATUS_ERROR) {
            std::string report = data[parts::MESSAGE];
            throw Client::error("error " + command + ": " + report);
        }
        handlers->execute(permission, command, *this, data);
    } catch (ClientHandlers::error &ex) {
        std::cerr << ex.what() << std::endl;
    } catch (Client::error &ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Client::session_output() {
    try {
        std::string command;
        std::getline(std::cin, command);
        commands->parse_and_execute(permission, command);
    } catch (ClientCommands::error &ex) {
        std::cerr << ex.what() << std::endl;
    } catch (Client::error &ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Client::login(const std::string &login, const std::string &password) {
    send(commands::SIGNIN, {
            {parts::LOGIN,    login},
            {parts::PASSWORD, password}
    });
}

void Client::logout() {
    send(commands::SINGOUT, {});
}

void Client::join(const std::string &name, const std::string &password) {
    send(commands::JOIN, {
            {parts::NAME,     name},
            {parts::PASSWORD, password}
    });
}

void Client::create(const std::string &name, const std::string &password) {
    send(commands::CREATE, {
            {parts::NAME,     name},
            {parts::PASSWORD, password}
    });
}

void Client::leave() {
    send(commands::LEAVE, {});
}

void Client::write(const std::string &message) {
    send(commands::WRITE, {
            {parts::MESSAGE, message}
    });
}

void Client::write(const std::string &login, const std::string &message) {
    send(commands::WRITE, {
            {parts::LOGIN,   login},
            {parts::MESSAGE, message}
    });
}

void Client::tables() {
    send(commands::TABLES, {});
}

void Client::users() {
    send(commands::USERS, {});
}

void Client::disconnect() {
    send(commands::DISCONNECT, {});
    stop();
}

void Client::sync() {
    send(commands::SYNC, {});
}

void Client::registration(const std::string &login, const std::string &password) {
    send(commands::SINGUP, {
            {parts::LOGIN,    login},
            {parts::PASSWORD, password}
    });
}

void Client::set_permission(const std::string &login, permission_t permission) {
    if (permission & ~permissions::WAIT)
        throw Client::error("changing permission on not WAIT permissions is impossible");
    send(commands::SET_PERMISSION, {
            {parts::LOGIN,     login},
            {parts::PERMITION, permission}
    });
}

void Client::send(const std::string &command, const json &data) {
    json request;
    request[parts::COMMAND] = command;
    request[parts::DATA] = data;
    send(request.dump());
}

void Client::bets() {
    send(commands::BETS, {});
}

void Client::bet(const std::string &type, int number, int value) {
    send(commands::BET, {
            {parts::TYPE,   type},
            {parts::NUMBER, number},
            {parts::VALUE,  value}
    });
}

void Client::spin() {
    send(commands::SPIN, {});

}

void Client::balance() {
    send(commands::BALANCE, {});
}

void Client::kick(const std::string &login) {
    send(commands::KICK, {
            {parts::LOGIN, login}
    });
}

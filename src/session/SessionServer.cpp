#include <algorithm>
#include <iostream>
#include "SessionServer.h"
#include "config.h"

SessionServer::SessionServer(std::shared_ptr<Socket> socket) : TransferServer(socket) {}

bool SessionServer::handle(identifier_t id, const std::string &message) {
    auto &&client = clients.find(id);
    auto &&session = client->second.first;
    auto &&state = client->second.second;
#ifdef DEBUG_ENABLE
     std::cout << "[DEBUG] ERECV " << message << std::endl;
 #endif
    auto &&decrypted = Session::unpack_and_decrypt_if_needed(*session, message);
#ifdef DEBUG_ENABLE
     std::cout << "[DEBUG] DRECV " << decrypted << std::endl;
 #endif
    switch (state) {
        case INIT: {
            state = READY;
            session->set_public_key(decrypted);
            send(id, crypto::INIT_MESSAGE);
            return false;
        }
        case READY: {
            return session_handle(id, decrypted);
        }
    }
}

void SessionServer::connect_handle(identifier_t id) {
    auto &&session = std::make_shared<Session>();
    clients.emplace(id, std::make_pair(session, INIT));
}

void SessionServer::disconnect_handle(identifier_t id) {
    clients.erase(id);
}

void SessionServer::raw_broadcast(const char *message) {
    std::string m(message);
    raw_broadcast(m);
}

void SessionServer::raw_broadcast(const std::string &message) {
    TransferServer::broadcast(crypto::RAW_MESSAGE_PREFIX + message);
}

void SessionServer::raw_send(identifier_t id, const char *message) {
    std::string m(message);
    raw_send(id, m);
}

void SessionServer::raw_send(identifier_t id, const std::string &message) {
    TransferServer::send(id, crypto::RAW_MESSAGE_PREFIX + message);
}

void SessionServer::raw_send(const std::vector<identifier_t> &ids, const char *message) {
    std::string m(message);
    raw_send(ids, m);
}

void SessionServer::raw_send(const std::vector<identifier_t> &ids, const std::string &message) {
    TransferServer::send(ids, crypto::RAW_MESSAGE_PREFIX + message);
}

void SessionServer::broadcast(const char *message) {
    std::string m(message);
    broadcast(m);
}

void SessionServer::broadcast(const std::string &message) {
    for (auto &&client : clients) {
        auto &&id = client.first;
        send(id, message);
    }
}

void SessionServer::send(identifier_t id, const char *message) {
    std::string m(message);
    send(id, m);
}

void SessionServer::send(identifier_t id, const std::string &message) {
#ifdef DEBUG_ENABLE
     std::cout << "[DEBUG] MSEND " << message << std::endl;
 #endif
    auto &&entry = clients.find(id);
    if (entry == std::end(clients)) return;
    auto &&session = entry->second.first;
    auto &&encrypted = Session::pack_and_encrypt_if_needed(*session, message);
#ifdef DEBUG_ENABLE
     std::cout << "[DEBUG] ESEND " << encrypted << std::endl;
 #endif
    TransferServer::send(id, encrypted);
}

void SessionServer::send(const std::vector<identifier_t> &ids, const char *message) {
    std::string m(message);
    send(ids, m);
}

void SessionServer::send(const std::vector<identifier_t> &ids, const std::string &message) {
    for (auto &&id : ids) {
        send(id, message);
    }
}

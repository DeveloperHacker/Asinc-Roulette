#include <algorithm>
#include "CryptoServer.h"
#include "config.h"

CryptoServer::CryptoServer(int domain, int type, int protocol, address_t &address
) : TCPServer(domain, type, protocol, address), clients() {
}

bool CryptoServer::handle(id_t id, address_t address, const std::string &message) {
    auto &&client = clients.find(id);
    auto &&transfer = client->second.first;
    auto &&state = client->second.second;
    auto &&decrypted = Transfer::parse_and_decrypt_if_needed(*transfer, message);
    switch (state) {
        case INIT: {
            state = READY;
            transfer->set_public_key(decrypted);
            send(id, crypto::INIT_MESSAGE);
            return false;
        }
        case READY: {
            return crypto_handle(id, address, decrypted);
        }
    }
}

void CryptoServer::connect_handle(id_t id, address_t address) {
    auto &&transfer = std::make_shared<Transfer>();
    clients.emplace(id, std::make_pair(transfer, INIT));
}

void CryptoServer::disconnect_handle(id_t id, address_t address) {
    clients.erase(id);
}

void CryptoServer::raw_broadcast(const char *message) {
    std::string m(message);
    raw_broadcast(m);
}

void CryptoServer::raw_broadcast(const std::string &message) {
    TCPServer::broadcast(crypto::RAW_MESSAGE_PREFIX + message);
}

void CryptoServer::raw_send(id_t id, const char *message) {
    std::string m(message);
    raw_send(id, m);
}

void CryptoServer::raw_send(id_t id, const std::string &message) {
    TCPServer::send(id, crypto::RAW_MESSAGE_PREFIX + message);
}

void CryptoServer::raw_send(const std::vector<id_t> &ids, const char *message) {
    std::string m(message);
    raw_send(ids, m);
}

void CryptoServer::raw_send(const std::vector<id_t> &ids, const std::string &message) {
    TCPServer::send(ids, crypto::RAW_MESSAGE_PREFIX + message);
}

void CryptoServer::broadcast(const char *message) {
    std::string m(message);
    broadcast(m);
}

void CryptoServer::broadcast(const std::string &message) {
    for (auto &&client : clients) {
        auto &&id = client.first;
        send(id, message);
    }
}

void CryptoServer::send(id_t id, const char *message) {
    std::string m(message);
    send(id, m);
}

void CryptoServer::send(id_t id, const std::string &message) {
    auto &&entry = clients.find(id);
    if (entry == std::end(clients)) return;
    auto &&transfer = entry->second.first;
    bool empty = message.empty();
    auto &&prefix = empty ? crypto::EMPTY_MESSAGE_PREFIX : crypto::NORMAL_MESSAGE_PREFIX;
    auto &&encrypted = transfer->encrypt(prefix + message);
    TCPServer::send(id, crypto::ENCRYPTED_MESSAGE_PREFIX + encrypted);
}

void CryptoServer::send(const std::vector<id_t> &ids, const char *message) {
    std::string m(message);
    send(ids, m);
}

void CryptoServer::send(const std::vector<id_t> &ids, const std::string &message) {
    for (auto &&id : ids) {
        send(id, message);
    }
}

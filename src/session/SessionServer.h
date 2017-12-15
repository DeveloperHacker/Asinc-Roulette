#pragma once

#include "../transfer/TransferServer.h"
#include "Session.h"

class SessionServer : public TransferServer {
public:
    enum State {
        INIT,
        READY
    };

private:
    std::unordered_map<identifier_t, std::pair<std::shared_ptr<Session>, State>> clients;

public:
    SessionServer(std::shared_ptr<Socket> socket);

    void broadcast(const char *message) override;

    void broadcast(const std::string &message) override;

    void send(identifier_t id, const char *message) override;

    void send(identifier_t id, const std::string &message) override;

    void send(const std::vector<identifier_t> &ids, const char *message) override;

    void send(const std::vector<identifier_t> &ids, const std::string &message) override;

    void raw_broadcast(const char *message);

    void raw_broadcast(const std::string &message);

    void raw_send(identifier_t id, const char *message);

    void raw_send(identifier_t id, const std::string &message);

    void raw_send(const std::vector<identifier_t> &ids, const char *message);

    void raw_send(const std::vector<identifier_t> &ids, const std::string &message);

protected:
    bool handle(identifier_t id, const std::string &message) override;

    void connect_handle(identifier_t id) override;

    void disconnect_handle(identifier_t id) override;

    virtual bool session_handle(identifier_t id, const std::string &message) = 0;
};

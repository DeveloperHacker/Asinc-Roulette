#include <condition_variable>
#include "../transfer/TransferClient.h"
#include "../../session/Transfer.h"
#include "Server.h"
#include "config.h"
#include "../../session/sessionClient.h"

class Client : public CryptoClient {
public:
    Client(int domain, int type, int protocol, address_t &address);

protected:
    void crypto_output() override;

    void crypto_input(const std::string &message) override;
};
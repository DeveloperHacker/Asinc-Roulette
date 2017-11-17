#include <condition_variable>
#include "../tcp/TCPClient.h"
#include "../../crypto/Transfer.h"
#include "Server.h"
#include "config.h"
#include "../../crypto/CryptoClient.h"

class Client : public CryptoClient {
public:
    Client(int domain, int type, int protocol, address_t &address);

protected:
    void crypto_output() override;

    void crypto_input(const std::string &message) override;
};
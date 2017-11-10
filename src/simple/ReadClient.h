#include <condition_variable>
#include "../../tcp/TCPClient.h"
#include "../Transfer.h"
#include "EchoServer.h"
#include "../global.h"

class ReadClient : public TCPClient {
public:
    enum State {
        INIT,
        WAIT,
        READY
    };

private:
    Transfer transfer;
    State state;

public:
    ReadClient(int domain, int type, int protocol, address_t &address);

protected:
    void output(SendSocket &socket) override;

    void input(const std::string &encrypted) override;
};
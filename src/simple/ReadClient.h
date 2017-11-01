#include <condition_variable>
#include "../../tcp/TCPClient.h"
#include "../Transfer.h"
#include "EchoServer.h"

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
    std::mutex mutex;
    std::condition_variable condvar;

public:
    ReadClient(int domain, int type, int protocol, address_t &address
    ) : TCPClient(domain, type, protocol, address), state(INIT) {
    }

protected:
    void output(SendSocket &socket) override {
        switch (state) {
            case INIT: {
                state = WAIT;
                socket.send(transfer.public_key());
            }
            case WAIT: {
                std::unique_lock<std::mutex> lock(mutex);
                condvar.wait(lock, [this] { return state == READY; });
                break;
            }
            case READY: {
                std::string message;
                std::getline(std::cin, message);
                socket.send(transfer.encrypt(message));
            }
        }
    };

    void input(const std::string &encrypted) override {
        switch (state) {
            case INIT: {
                throw std::runtime_error("Protocol Error");
            }
            case WAIT: {
                auto &&message = transfer.decrypt(encrypted);
                std::cout << "Message receive '" << message << "'" << std::endl;
                if (message == INIT_MESSAGE) break;
                throw std::runtime_error("Protocol Error");
            }
            case READY: {
                auto &&message = transfer.decrypt(encrypted);
                std::cout << "Message receive '" << message << "'" << std::endl;
                break;
            }
        }
    }
};
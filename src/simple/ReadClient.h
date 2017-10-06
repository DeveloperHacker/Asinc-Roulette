#include "../../tcp/TCPClient.h"

class ReadClient : public TCPClient {
public:
    ReadClient(int domain, int type, int protocol, address_t &address) :
            TCPClient(domain, type, protocol, address) {}

protected:
    Task output() override {
        std::string message;
        std::getline(std::cin, message);
        return Task(message);
    };

    void input(const Task &task) override {
        std::cout << "report: '" << task.message << "'" << std::endl;
    }
};
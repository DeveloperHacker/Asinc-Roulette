#include <iostream>
#include "src/Socket.h"
#include "src/TCPServer.h"
#include <sstream>
#include <vector>
#include <iterator>
#include <arpa/inet.h>

template<typename Out>
void split(const std::string &s, char delimiter, Out result, bool skip) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        if (!skip || item.length() > 0) *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delimiter, bool skip = true) {
    std::vector<std::string> strings;
    split(s, delimiter, std::back_inserter(strings), skip);
    return strings;
}

class Server : public TCPServer {
public:
    Server(int domain, int type, int protocol, address_t &address) :
            TCPServer(domain, type, protocol, address) {}

    bool handle(SafeSocket &socket) override {
        std::string message = socket.receive(0);
        std::cout << "received: '" << message << "'" << std::endl;
        socket.send(message, 0);
        return message == "close";
    }
};

int main() {
    address_t address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(7777);
    address.sin_addr.s_addr = INADDR_ANY;

    Server server(AF_INET, SOCK_STREAM, 0, address);
    while (!server.is_stopped()) {
        std::string command;
        std::getline(std::cin, command);
        auto &&arguments = split(command, ' ');
        if (arguments.size() == 1 && arguments[0] == "shutdown") {
            server.stop();
        } else if (arguments.size() == 2 && arguments[0] == "kill") {
            std::istringstream stream(arguments[1]);
            socket_t descriptor = 0;
            stream >> descriptor;
            if (!stream.fail()) {
                server.kill(descriptor);
            } else if (arguments[1] == "all") {
                auto &&descriptors = server.get_descriptors();
                for (auto &&entry: descriptors) server.kill(entry.first);
            } else {
                std::cerr << "argument '" << arguments[1] << "' hasn't recognized" << std::endl;
            }
        } else if (arguments.size() == 1 && arguments[0] == "list") {
            auto &&descriptors = server.get_descriptors();
            for (auto &&entry: descriptors) {
                auto &&descriptor = entry.first;
                auto &&client_address = entry.second;
                auto &&host = inet_ntoa(client_address.sin_addr);
                auto &&port = client_address.sin_port;
                printf("%-4d %10s:%d\n", descriptor, host, port);
            }
        } else {
            std::cerr << "command '" << command << "' hasn't recognized" << std::endl;
        }
    }
    server.join();
}
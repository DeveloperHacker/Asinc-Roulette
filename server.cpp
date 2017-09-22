#include <iostream>
#include "src/Socket.h"
#include "src/TCPServer.h"
#include <sstream>
#include <vector>
#include <iterator>

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
            std::cout << "shutdown inited" << std::endl;
            break;
        } else if (!arguments.empty() && arguments[0] == "kill") {
            if (arguments.size() != 2) {
                std::cerr << "kill arguments not found" << std::endl;
                continue;
            }
            try {
                socket_t descriptor = std::stoi(arguments[1]);
                int stat = server.kill(descriptor);
                std::cout << "connection '" << arguments[1];
                switch (stat) {
                    case 1: {
                        std::cout << "' hasn't exist" << std::endl;
                        break;
                    }
                    case 0: {
                        std::cout << "' closed" << std::endl;
                        break;
                    }
                    case -1: {
                        std::cout << "' already closed" << std::endl;
                        break;
                    }
                    default:
                        break;
                }
            } catch (...) {
                std::cerr << "descriptor '" << arguments[1] << "' is not number" << std::endl;
            }
        } else if (arguments.size() == 1 && arguments[0] == "list") {
            auto &&descriptors = server.get_descriptors();
            if (descriptors.empty()) std::cout << "connections hasn't found" << std::endl;
            for (auto &&entry: descriptors) {
                auto &&descriptor = entry.first;
                // auto &&address = entry.second;
                std::cout << descriptor << std::endl;
            }
        } else {
            std::cerr << "command '" << command << "' hasn't recognized" << std::endl;
        }
    }
    server.join();
}

#include "config.h"
#include "core/Client.h"

#ifdef _WIN32
    #include "transfer/WinTCPSocket.h"
#else
    #include "transfer/LinuxTCPSocket.h"
#endif

int main() {
#ifdef _WIN32
    WinTCPSocket::startup();
    auto &&address = WinTCPSocket::make_address(AF_INET, SOCK_STREAM, 0, address::SERVER_HOST, address::SERVER_PORT);
#else
    auto &&address = LinuxTCPSocket::make_address(address::SERVER_HOST, address::SERVER_PORT);
    auto &&socket = std::make_shared<LinuxTCPSocket>(AF_INET, SOCK_STREAM, 0);
#endif
    socket->connect(address);
    Client client(socket);
    client.start();
    client.join();
#ifdef _WIN32
    WinTCPSocket::cleanup();
#endif
}
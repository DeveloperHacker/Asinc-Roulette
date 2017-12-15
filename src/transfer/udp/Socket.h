#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
    #include "WinSocket.h"
    using socket_t = SOCKET;
    using address_t = addrinfo*;
    using Socket = WinSocket;
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <iostream>
    #include "LinuxSocket.h"
    using socket_t = int;
    using address_t = sockaddr_in;
    using Socket = LinuxSocket;
#endif
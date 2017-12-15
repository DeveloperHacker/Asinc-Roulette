#pragma once


#ifdef _WIN32
    using socket_t = SOCKET;
    using address_t = addrinfo*;
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include "LinuxSocket.h"
    using socket_t = int;
    using address_t = sockaddr_in;
    using Socket = LinuxSocket;
#endif

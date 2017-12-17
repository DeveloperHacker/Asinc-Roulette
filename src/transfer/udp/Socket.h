#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
    #include "WinSocket.h"
    using Socket = WinSocket;
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <iostream>
    #include "LinuxSocket.h"
    using Socket = LinuxSocket;
#endif
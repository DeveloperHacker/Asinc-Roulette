#pragma once

#ifdef _WIN32
    #include "WinSocket.h"
    using Socket = WinSocket;
#else
    #include "LinuxSocket.h"
    using Socket = LinuxSocket;
#endif

#pragma once

#ifdef TCP
    #include "tcp/Socket.h"
#elif UDP
    #include "udp/Socket.h"
#else
    #error
#endif

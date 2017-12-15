#pragma once

#ifdef TCP
    #include "tcp/TransferServer.h"
#elif UDP
    #include "udp/TransferServer.h"
#else
    #error
#endif

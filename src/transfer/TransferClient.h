#pragma once

#ifdef TCP
    #include "tcp/TransferClient.h"
#elif UDP
    #include "udp/TransferClient.h"
#else
    #error
#endif
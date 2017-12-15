cmake_minimum_required(VERSION 3.8)

project(async_roulette)

set(CMAKE_CXX_STANDARD 14)
set(TYPE UDP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUDP")

set(JSON lib/json/src/json.hpp)
set(CRYPTO lib/crypto/Crypto.cpp lib/crypto/Crypto.h)
set(BASE64 lib/base64/base64.cpp lib/base64/base64.h)
set(POOL lib/thread_pool/ThreadPool.h)

add_subdirectory(lib/SQLiteCpp)
include_directories(lib)


if (WIN32)
    if (${TYPE} STREQUAL UDP)
        set(SOCKET src/transfer/udp/Socket.h src/transfer/udp/WinSocket.cpp src/transfer/udp/WinSocket.h)
    elseif (${TYPE} STREQUAL TCP)
        set(SOCKET src/transfer/tcp/Socket.h src/transfer/tcp/WinSocket.cpp src/transfer/tcp/WinSocket.h)
    endif ()
else ()
    if (${TYPE} STREQUAL UDP)
        set(SOCKET src/transfer/udp/Socket.h src/transfer/udp/LinuxSocket.cpp src/transfer/udp/LinuxSocket.h)
    elseif (${TYPE} STREQUAL TCP)
        set(SOCKET src/transfer/tcp/Socket.h src/transfer/tcp/LinuxSocket.cpp src/transfer/tcp/LinuxSocket.h)
    endif ()
endif ()

set(SOCKET src/transfer/Socket.h ${SOCKET})

if (${TYPE} STREQUAL UDP)
    set(TRANSFER_SERVER src/transfer/TransferServer.h src/transfer/udp/TransferServer.h src/transfer/udp/TransferServer.cpp ${SOCKET} ${POOL})
    set(TRANSFER_CLIENT src/transfer/TransferClient.h src/transfer/udp/TransferClient.h src/transfer/udp/TransferClient.cpp ${SOCKET})
elseif (${TYPE} STREQUAL TCP)
    set(TRANSFER_SERVER src/transfer/TransferServer.h src/transfer/tcp/TransferServer.h src/transfer/tcp/TransferServer.cpp ${SOCKET} ${POOL})
    set(TRANSFER_CLIENT src/transfer/TransferClient.h src/transfer/tcp/TransferClient.h src/transfer/tcp/TransferClient.cpp ${SOCKET})
endif ()

set(STRINGS src/session/strings.h)

set(SESSION src/session/Session.cpp src/session/Session.h ${CRYPTO} ${BASE64} ${JSON} ${STRINGS})
set(SESSION_SERVER src/session/config.h src/session/SessionServer.cpp src/session/SessionServer.h ${TRANSFER_SERVER} ${SESSION})
set(SESSION_CLIENT src/session/config.h src/session/SessionClient.cpp src/session/SessionClient.h ${TRANSFER_CLIENT} ${SESSION})

set(ECHO_SERVER src/simple/Server.cpp src/simple/Server.h ${SESSION_SERVER})
set(READ_CLIENT src/simple/Client.cpp src/simple/Client.h ${SESSION_CLIENT} src/session/config.h)

set(DATABASE src/core/DataBase.cpp src/core/DataBase.h)
set(SERVER src/core/config.h src/core/Server.cpp src/core/Server.h ${SESSION_SERVER} ${DATABASE} ${JSON})
set(CLIENT src/core/config.h src/core/Client.cpp src/core/Client.h ${SESSION_CLIENT} ${JSON})

set(COMMAND src/commands/Command.cpp src/commands/Command.h ${STRINGS})
set(COMMANDS src/commands/Commands.cpp src/commands/Commands.h src/commands/permissions.h ${COMMAND})
set(SIMPLE_SERVER_COMMANDS src/simple/SimpleServerCommands.h ${COMMANDS} ${TRANSFER_SERVER})
set(SERVER_COMMANDS src/commands/ServerCommands.h ${SIMPLE_SERVER_COMMANDS})
set(CLIENT_COMMANDS src/commands/ClientCommands.cpp src/commands/ClientCommands.h ${COMMANDS} ${CLIENT})

set(HANDLERS src/handlers/Handlers.h)
set(CLIENT_HANDLERS src/handlers/ClientHandlers.cpp src/handlers/ClientHandlers.h ${HANDLERS} ${CLIENT} ${JSON})
set(SERVER_HANDLERS src/handlers/ServerHandlers.cpp src/handlers/ServerHandlers.h ${HANDLERS} ${SERVER} ${JSON})

add_executable(playground src/playground.cpp ${SESSION})
add_executable(server src/config.h src/server.cpp ${SOCKET} ${SERVER} ${SERVER_COMMANDS} ${SERVER_HANDLERS})
add_executable(client src/config.h src/client.cpp ${SOCKET} ${CLIENT} ${CLIENT_COMMANDS} ${CLIENT_HANDLERS})
add_executable(init src/config.h src/init.cpp ${DATABASE})

if (WIN32)
    target_link_libraries(server ws2_32 wsock32 crypto SQLiteCpp sqlite3)
    target_link_libraries(client ws2_32 wsock32 crypto)
    target_link_libraries(init SQLiteCpp sqlite3)
else ()
    target_link_libraries(server pthread crypto dl SQLiteCpp sqlite3)
    target_link_libraries(client pthread crypto)
    target_link_libraries(init pthread dl SQLiteCpp sqlite3)
endif ()

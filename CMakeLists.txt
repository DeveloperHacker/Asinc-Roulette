cmake_minimum_required(VERSION 3.8)

project(async_roulette)

set(CMAKE_CXX_STANDARD 14)

set(JSON lib/json/src/json.hpp)
set(CRYPTO lib/crypto/Crypto.cpp lib/crypto/Crypto.h)
set(BASE64 lib/base64/base64.cpp lib/base64/base64.h)
set(POOL lib/thread_pool/ThreadPool.h)

include_directories(lib)

if(WIN32)
    set(SOCKET src/tcp/Socket.h src/tcp/WinSocket.cpp src/tcp/WinSocket.h)
else()
    set(SOCKET src/tcp/Socket.h src/tcp/LinuxTCPSocket.cpp src/tcp/LinuxTCPSocket.h)
endif()

set(TRANSFER_SERVER src/tcp/TransferServer.cpp src/tcp/TransferServer.h ${POOL} ${SOCKET})
set(TRANSFER_CLIENT src/tcp/TransferClient.h src/tcp/TransferClient.cpp ${SOCKET})

set(STRINGS src/crypto/strings.h)

set(SESSION src/crypto/Session.cpp src/crypto/Session.h ${CRYPTO} ${BASE64} ${JSON} ${STRINGS})
set(SESSION_SERVER src/crypto/config.h src/crypto/SessionServer.cpp src/crypto/SessionServer.h ${TRANSFER_SERVER} ${SESSION})
set(SESSION_CLIENT src/crypto/config.h src/crypto/SessionClient.cpp src/crypto/SessionClient.h ${TRANSFER_CLIENT} ${SESSION})

set(ECHO_SERVER src/simple/Server.cpp src/simple/Server.h ${SESSION_SERVER})
set(READ_CLIENT src/simple/Client.cpp src/simple/Client.h ${SESSION_CLIENT} src/crypto/config.h)

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
#add_executable(server simple/config.h simple/server.cpp ${ECHO_SERVER} ${SIMPLE_SERVER_COMMANDS})
#add_executable(client simple/config.h simple/client.cpp ${READ_CLIENT})
add_executable(server src/config.h src/server.cpp ${SERVER} ${SERVER_COMMANDS} ${SERVER_HANDLERS})
add_executable(client src/config.h src/client.cpp ${CLIENT} ${CLIENT_COMMANDS} ${CLIENT_HANDLERS})
add_executable(init src/config.h src/init.cpp ${DATABASE})

if (WIN32)
    target_link_libraries(client ws2_32 wsock32)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lcrypto -lsqlite3 -g3")
endif()

target_link_libraries(server /usr/local/lib/libSQLiteCpp.a)
target_link_libraries(init /usr/local/lib/libSQLiteCpp.a)

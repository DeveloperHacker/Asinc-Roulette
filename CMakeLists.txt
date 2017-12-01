cmake_minimum_required(VERSION 3.8)

project(async_roulette)

set(CMAKE_CXX_STANDARD 14)

set(JSON lib/json/src/json.hpp)
set(CRYPTO lib/crypto/Crypto.cpp lib/crypto/Crypto.h)
set(BASE64 lib/base64/base64.cpp lib/base64/base64.h)
set(POOL lib/thread_pool/ThreadPool.h)

if(WIN32)
    set(SOCKET src/tcp/Socket.h src/tcp/WinSocket.cpp src/tcp/WinSocket.h)
else()
    set(SOCKET src/tcp/Socket.h src/tcp/LinuxSocket.cpp src/tcp/LinuxSocket.h)
endif()

set(TCP_SERVER src/tcp/TCPServer.cpp src/tcp/TCPServer.h ${POOL} ${SOCKET})
set(TCP_CLIENT src/tcp/TCPClient.h src/tcp/TCPClient.cpp ${SOCKET})

set(STRINGS src/crypto/strings.h)

set(TRANSFER src/crypto/Transfer.cpp src/crypto/Transfer.h ${BASE64} ${STRINGS})
set(CRYPTO_SERVER src/crypto/config.h src/crypto/CryptoServer.cpp src/crypto/CryptoServer.h ${TCP_SERVER} ${TRANSFER})
set(CRYPTO_CLIENT src/crypto/config.h src/crypto/CryptoClient.cpp src/crypto/CryptoClient.h ${TCP_CLIENT} ${TRANSFER})

set(ECHO_SERVER src/simple/Server.cpp src/simple/Server.h ${CRYPTO_SERVER})
set(READ_CLIENT src/simple/Client.cpp src/simple/Client.h ${CRYPTO_CLIENT} src/crypto/config.h)

set(DATABASE src/core/DataBase.cpp src/core/DataBase.h)
set(SERVER src/core/Server.cpp src/core/Server.h ${CRYPTO_SERVER} ${DATABASE} ${JSON})
set(CLIENT src/core/Client.cpp src/core/Client.h ${CRYPTO_CLIENT} ${JSON})

set(COMMAND src/commands/Command.cpp src/commands/Command.h ${STRINGS})
set(COMMANDS src/commands/Commands.cpp src/commands/Commands.h src/commands/permissions.h ${COMMAND})
set(SIMPLE_SERVER_COMMANDS src/simple/SimpleServerCommands.h ${COMMANDS} ${TCP_SERVER})
set(SERVER_COMMANDS src/commands/ServerCommands.h ${SIMPLE_SERVER_COMMANDS})
set(CLIENT_COMMANDS src/commands/ClientCommands.cpp src/commands/ClientCommands.h ${COMMANDS} ${CLIENT})

set(HANDLERS src/handlers/Handlers.h)
set(CLIENT_HANDLERS src/handlers/ClientHandlers.cpp src/handlers/ClientHandlers.h ${HANDLERS} ${CLIENT} ${JSON})
set(SERVER_HANDLERS src/handlers/ServerHandlers.cpp src/handlers/ServerHandlers.h ${HANDLERS} ${SERVER} ${JSON})

add_executable(playground src/playground.cpp ${TRANSFER})
#add_executable(server simple/config.h simple/server.cpp ${ECHO_SERVER} ${SIMPLE_SERVER_COMMANDS})
#add_executable(client simple/config.h simple/client.cpp ${READ_CLIENT})
add_executable(server src/core/config.h src/server.cpp ${SERVER} ${SERVER_COMMANDS} ${SERVER_HANDLERS})
add_executable(client src/core/config.h src/client.cpp ${CLIENT} ${CLIENT_COMMANDS} ${CLIENT_HANDLERS})
add_executable(init src/core/config.h src/init.cpp ${DATABASE})

if (WIN32)
    target_link_libraries(client ws2_32 wsock32)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -g3")
endif()

target_link_libraries(server /usr/local/lib/libSQLiteCpp.a)
target_link_libraries(init /usr/local/lib/libSQLiteCpp.a)

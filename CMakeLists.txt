cmake_minimum_required(VERSION 3.8)

project(async_roulette)

# set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lcrypto -lsqlite3 -g3")

set(JSON lib/json/src/json.hpp)
set(CRYPTO lib/crypto/base64.cpp lib/crypto/base64.h lib/crypto/Crypto.cpp lib/crypto/Crypto.h)
set(POOL lib/thread_pool/ThreadPool.h)

set(LINUX_SOCKET tcp/LinuxSocket.cpp tcp/LinuxSocket.h)
set(SOCKET tcp/Socket.h ${LINUX_SOCKET})
set(TCP_SERVER tcp/TCPServer.cpp tcp/TCPServer.h ${POOL} ${SOCKET})
set(TCP_CLIENT tcp/TCPClient.h tcp/TCPClient.cpp ${SOCKET})

set(STRINGS crypto/strings.h)

set(TRANSFER crypto/Transfer.cpp crypto/Transfer.h ${CRYPTO} ${JSON} ${STRINGS})
set(CRYPTO_SERVER crypto/config.h crypto/CryptoServer.cpp crypto/CryptoServer.h ${TCP_SERVER} ${TRANSFER})
set(CRYPTO_CLIENT crypto/config.h crypto/CryptoClient.cpp crypto/CryptoClient.h ${TCP_CLIENT} ${TRANSFER})

set(ECHO_SERVER simple/Server.cpp simple/Server.h ${CRYPTO_SERVER})
set(READ_CLIENT simple/Client.cpp simple/Client.h ${CRYPTO_CLIENT} crypto/config.h)

set(DATABASE src/core/DataBase.cpp src/core/DataBase.h)
set(SERVER src/Server.cpp src/Server.h ${CRYPTO_SERVER} ${DATABASE} ${JSON})
set(CLIENT src/Client.cpp src/Client.h ${CRYPTO_CLIENT} ${JSON})

set(COMMAND commands/Command.cpp commands/Command.h ${STRINGS})
set(COMMANDS commands/Commands.cpp commands/Commands.h commands/permitions.h ${COMMAND})
set(SIMPLE_SERVER_COMMANDS simple/SimpleServerCommands.h ${COMMANDS} ${TCP_SERVER})
set(SERVER_COMMANDS src/commands/ServerCommands.h ${SIMPLE_SERVER_COMMANDS})
set(CLIENT_COMMANDS src/commands/ClientCommands.cpp src/commands/ClientCommands.h ${COMMANDS} ${CLIENT})

set(HANDLERS src/handlers/Handlers.h)
set(CLIENT_HANDLERS src/handlers/ClientHandlers.cpp src/handlers/ClientHandlers.h ${HANDLERS} ${CLIENT})
set(SERVER_HANDLERS src/handlers/ServerHandlers.cpp src/handlers/ServerHandlers.h ${HANDLERS} ${SERVER})

#add_executable(server simple/config.h simple/server.cpp ${ECHO_SERVER} ${SIMPLE_SERVER_COMMANDS})
#add_executable(client simple/config.h simple/client.cpp ${READ_CLIENT})
add_executable(server src/config.h src/server.cpp ${SERVER} ${SERVER_COMMANDS} ${SERVER_HANDLERS})
add_executable(client src/config.h src/client.cpp ${CLIENT} ${CLIENT_COMMANDS} ${CLIENT_HANDLERS})
#add_executable(playground simple/playgound.cpp ${TRANSFER})
add_executable(init src/config.h src/init.cpp ${DATABASE})

target_link_libraries(server /usr/local/lib/libSQLiteCpp.a)
target_link_libraries(init /usr/local/lib/libSQLiteCpp.a)

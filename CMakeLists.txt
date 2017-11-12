cmake_minimum_required(VERSION 3.8)

project(async_roulette)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g3 -lcrypto")

set(JSON lib/json/src/json.hpp)
set(CRYPTO lib/crypto/base64.cpp lib/crypto/base64.h lib/crypto/Crypto.cpp lib/crypto/Crypto.h)
set(POOL lib/thread_pool/ThreadPool.h)

set(SOCKET tcp/Socket.cpp tcp/Socket.h)
set(TCP_SERVER tcp/TCPServer.cpp tcp/TCPServer.h ${POOL} ${SOCKET})
set(TCP_CLIENT tcp/TCPClient.h tcp/TCPClient.cpp ${SOCKET})

set(TRANSFER crypto/Transfer.cpp crypto/Transfer.h ${CRYPTO} ${JSON})
set(CRYPTO_SERVER crypto/CryptoServer.cpp crypto/CryptoServer.h crypto/config.h ${TCP_SERVER} ${TRANSFER})
set(CRYPTO_CLIENT crypto/CryptoClient.cpp crypto/CryptoClient.h crypto/config.h ${TCP_CLIENT} ${TRANSFER})

set(ECHO_SERVER simple/EchoServer.h simple/EchoServer.cpp ${CRYPTO_SERVER})
set(READ_CLIENT simple/ReadClient.h simple/ReadClient.cpp ${CRYPTO_CLIENT} crypto/config.h)

set(STATE src/game/State.h)
set(TABLE src/game/Table.h)
set(DB src/game/DataBase.h)
set(ROLE src/game/Role.h)
set(SERVER src/game/Server.h src/game/Server.cpp ${CRYPTO_SERVER} ${ROLE} ${STATE})
set(CLINET src/game/Client.h src/game/Client.cpp ${CRYPTO_CLIENT} ${DB} ${TABLE} ${STATE})

set(COMMAND src/Command.h src/strings.h)

add_executable(server src/server.cpp simple/config.h ${ECHO_SERVER} ${COMMAND})

add_executable(client src/client.cpp simple/config.h ${READ_CLIENT})

add_executable(playground src/playgound.cpp ${TRANSFER})

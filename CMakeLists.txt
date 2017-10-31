cmake_minimum_required(VERSION 3.8)

project(async_roulette)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g3 -lcrypto")

set(SOCKET tcp/Socket.cpp tcp/Socket.h)

set(THREAD_POOL tcp/ThreadPool.h)

set(SAFE_SOCKET tcp/SafeSocket.h ${SOCKET})

set(TCP_SERVER tcp/TCPServer.cpp tcp/TCPServer.h ${THREAD_POOL} ${SAFE_SOCKET})

set(TCP_CLIENT tcp/TCPClient.h tcp/TCPClient.cpp ${SOCKET})

set(ECHO_SERVER src/simple/EchoServer.h ${TCP_SERVER})

set(READ_CLIENT src/simple/ReadClient.h ${TCP_CLIENT})

set(STATE src/game/State.h)

set(TABLE src/game/Table.h)

set(DB src/game/DataBase.h)

set(ROLE src/game/Role.h)

set(JSON lib/json/src/json.hpp)

set(CRYPTO lib/crypto/base64.cpp lib/crypto/base64.h lib/crypto/Crypto.cpp lib/crypto/Crypto.h)

set(TRANSFER src/Transfer.cpp src/Transfer.h ${CRYPTO} ${JSON})

set(SERVER src/game/Server.h src/game/Server.cpp ${ROLE} ${TCP_SERVER} ${STATE} ${TRANSFER})

set(CLINET src/game/Client.h src/game/Client.cpp ${DB} ${TABLE} ${TCP_CLIENT} ${STATE} ${TRANSFER})

set(CONFIG src/Config.h)

set(COMMAND src/Command.h)

add_executable(server src/server.cpp ${CONFIG} ${SERVER} ${ECHO_SERVER} ${COMMAND})

add_executable(client src/client.cpp ${CONFIG} ${CLINET} ${READ_CLIENT})

add_executable(playground src/playgound.cpp ${TRANSFER})

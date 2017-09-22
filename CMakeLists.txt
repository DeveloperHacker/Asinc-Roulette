cmake_minimum_required(VERSION 3.8)

project(client_server)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g2 -O0")

set(SOCKET tcp/Socket.cpp tcp/Socket.h)

set(THREAD_POOL tcp/ThreadPool.h)

set(TCP_SERVER ${SOCKET} ${THREAD_POOL} tcp/TCPServer.cpp tcp/TCPServer.h tcp/SafeSocket.h src/Command.h)

add_executable(server src/server.cpp ${TCP_SERVER} src/server.cpp src/Server.h)

add_executable(client src/client.cpp ${SOCKET})

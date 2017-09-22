cmake_minimum_required(VERSION 3.8)

project(client_server)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g2 -O0")

set(SOCKET src/Socket.cpp src/Socket.h)

set(THREAD_POOL src/ThreadPool.h)

set(TCP_SERVER ${SOCKET} ${THREAD_POOL} src/TCPServer.cpp src/TCPServer.h src/SafeSocket.h)

add_executable(server server.cpp ${TCP_SERVER} server.cpp)

add_executable(client client.cpp ${SOCKET})

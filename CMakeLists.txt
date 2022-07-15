# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.18)
project(MyServer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE "Debug")

set(CXX_FLAGS
        -O3
        -g3
        -ggdb3
        -Wall
        -std=c++11
        -lpthread
        -Wno-unused-parameter
        )

link_libraries(pthread)

add_executable(MyServer main.cpp
        src/Threadpool.cpp
        src/Socket.cpp
        src/Server.cpp
        src/Epoll.cpp
        src/Log.cpp
        src/Timer.cpp
        src/Httpdata.cpp
        src/HttpRequest.cpp
        src/HttpReponse.cpp
        src/HttpParser.cpp
        )
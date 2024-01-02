cmake_minimum_required(VERSION 3.18)
project(ServerHad)

set(CMAKE_CXX_STANDARD 11)

add_executable(ServerHad main.cpp
        Gamer.cpp
        Gamer.h)
target_link_libraries(ServerHad pthread)

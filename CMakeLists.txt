cmake_minimum_required(VERSION 3.7)
project(Coursework2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        client.c
        server.c)

add_executable(Coursework2 ${SOURCE_FILES})
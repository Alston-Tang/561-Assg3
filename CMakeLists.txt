cmake_minimum_required(VERSION 3.5.1)
project(561Assg3)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
        main.cpp
        ast.h global.h ast.cpp)

add_executable(561Assg3 ${SOURCE_FILES})
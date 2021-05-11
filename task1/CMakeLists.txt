cmake_minimum_required(VERSION 3.16)
project(task1)

set(CMAKE_CXX_STANDARD 11)

add_executable(task1 main.cpp)

set(CMAKE_CXX_FLAGS   "-O3 -pthread  " )
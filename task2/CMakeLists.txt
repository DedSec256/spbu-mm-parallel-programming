cmake_minimum_required(VERSION 3.16)
project(task2)

set(CMAKE_CXX_STANDARD 17)

add_executable(task2 main.cpp)

set(CMAKE_CXX_FLAGS   "-O3 -pthread  -Wall " )
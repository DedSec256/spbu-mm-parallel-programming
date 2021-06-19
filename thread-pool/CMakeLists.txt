cmake_minimum_required(VERSION 3.19)
project(thread_pool)

set(CMAKE_CXX_STANDARD 17)

add_executable(thread_pool main.cpp Task.h ThreadPool.h TestRunner.h)
cmake_minimum_required(VERSION 3.19)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 11)

add_executable(ThreadPool main.cpp src/ThreadPool.cpp src/ThreadPool.h src/IMyTask.cpp src/IMyTask.h)
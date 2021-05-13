cmake_minimum_required(VERSION 3.15)
project(producer_consumer)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES Buffer.h Worker.h Producer.h Consumer.h TestRunner.h)

add_executable(producer_consumer main.cpp ${SOURCES})
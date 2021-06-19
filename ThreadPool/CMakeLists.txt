cmake_minimum_required(VERSION 3.19)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 17)

#aux_source_directory(other OTHER_SOURCE_FILES)
#FILE(GLOB OTHER_HEADER other/*.h)
#FILE(GLOB OTHER_SOURCE other/*.cpp)

#include_directories(other)
include_directories(src)

#message(${OTHER_SOURCE_FILES})
#message(${OTHER_HEADER})
#message(${OTHER_SOURCE})

add_executable(hbk main.cpp src/ThreadPool.cpp src/ThreadPool.h src/IMyTask.cpp src/IMyTask.h)
#add_executable(other ${OTHER_SOURCE} ${OTHER_HEADER})
cmake_minimum_required(VERSION 3.30)
project(MemPool)

set(CMAKE_CXX_STANDARD 23)

add_library(MemPool STATIC src/MemPool.cpp)

add_executable(Test test.cpp)
target_link_libraries(Test MemPool)
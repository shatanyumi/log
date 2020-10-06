cmake_minimum_required(VERSION 3.17)
project(log)

set(CMAKE_CXX_STANDARD 20)

add_executable(log main.cpp includeLockLog/locklog.cpp includeLockLog/locklog.h)
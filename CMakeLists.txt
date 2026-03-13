cmake_minimum_required(VERSION 4.2)
project(NormalKoi)

set(CMAKE_CXX_STANDARD 17)

add_executable(NormalKoi koiLib.cpp)
set(CMAKE_CXX_FLAGS "-g -DLOCAL -Wall -lm -static")

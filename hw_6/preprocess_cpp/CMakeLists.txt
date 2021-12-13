cmake_minimum_required(VERSION 3.20)
project(DataToDistances)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")

add_executable(DataToDistances main.cpp data.cpp data.h)

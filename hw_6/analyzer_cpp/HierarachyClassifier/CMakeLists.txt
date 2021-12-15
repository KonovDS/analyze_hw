cmake_minimum_required(VERSION 3.20)
project(Cluster)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")

add_executable(Cluster main.cpp Data.cpp Data.h HierarchyClassifier.cpp HierarchyClassifier.h)

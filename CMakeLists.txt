cmake_minimum_required(VERSION 3.27)
project(btree)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(btree
        btree.h
        main.cpp
        node.h
        tester.h)

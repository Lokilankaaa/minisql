cmake_minimum_required(VERSION 3.15)
project(minisql)

set(CMAKE_CXX_STANDARD 14)

add_executable(minisql main.cpp src/API.h src/API.cpp src/interpreter.cpp src/interpreter.h src/Catalog_manager.cpp src/Catalog_manager.h)
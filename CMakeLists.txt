cmake_minimum_required(VERSION 3.15)
project(minisql)

set(CMAKE_CXX_STANDARD 14)

add_executable(minisql main.cpp src/API.h src/API.cpp src/interpreter.cpp src/interpreter.h src/Catalog_manager.cpp src/Catalog_manager.h src/Grammar.cpp src/Grammar.h src/meta.h src/meta.cpp src/error.h src/buffer_manager.cpp src/buffer_manager.h src/buffer_manager_page.h src/buffer_manager_page.cpp)
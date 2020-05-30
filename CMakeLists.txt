cmake_minimum_required(VERSION 3.15)
project(minisql)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(src DIR_SRC)

add_executable(${PROJECT_NAME} main.cpp ${DIR_SRC})
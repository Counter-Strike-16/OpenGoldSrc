cmake_minimum_required(VERSION 2.8)
project(OGS-launcher)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(hl ${PROJECT_SOURCES})
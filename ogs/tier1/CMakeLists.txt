cmake_minimum_required(VERSION 2.8)
project(OGS-tier1)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(tier1 STATIC ${PROJECT_SOURCES})
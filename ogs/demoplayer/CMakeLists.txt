cmake_minimum_required(VERSION 2.8)
project(OGS-demoplayer)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(DemoPlayer SHARED ${PROJECT_SOURCES})
cmake_minimum_required(VERSION 2.8)
project(OGS-tier0)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(tier0 SHARED ${PROJECT_SOURCES})
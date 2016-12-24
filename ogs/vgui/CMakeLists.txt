cmake_minimum_required(VERSION 2.8)
project(OGS-vgui)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(vgui SHARED ${PROJECT_SOURCES})
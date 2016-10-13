cmake_minimum_required(VERSION 2.8)
project(ogs-engine)

include_directories(.)

file(GLOB_RECURSE ENGINE_SOURCES .cpp .c)

add_library(hw SHARED ${ENGINE_SOURCES})

target_link_libraries()
cmake_minimum_required(VERSION 2.8)
project(ogs-engine)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

file(GLOB_RECURSE ENGINE_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	)

add_library(hw SHARED ${ENGINE_SOURCES}) # or engine

target_link_libraries()
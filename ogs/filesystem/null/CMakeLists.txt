cmake_minimum_required(VERSION 2.8)
project(ogs-filesystem_null)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

file(GLOB_RECURSE PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	)

add_library(filesystem_null SHARED ${PROJECT_SOURCES})
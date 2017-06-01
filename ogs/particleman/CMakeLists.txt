cmake_minimum_required(VERSION 2.8)
project(OGS-particleman)

# Output name
set(MODULE_OUTPUT_FILENAME particleman)

# Build a list of headers
file(GLOB_RECURSE PROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Build a list of sources
file(GLOB_RECURSE PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Build as dynamic library
add_library(${MODULE_OUTPUT_FILENAME} SHARED ${PROJECT_HEADERS} ${PROJECT_SOURCES})
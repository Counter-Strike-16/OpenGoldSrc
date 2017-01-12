cmake_minimum_required(VERSION 2.8)
project(OGS-engine)

# Output name
set(MODULE_OUTPUT_FILENAME engine)

# Version config file
#configure_file(version.h.in version.hpp)

# Enable SSE4.1 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")

# Add include directories
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/
)

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

# Link with additional libraries
target_link_libraries()
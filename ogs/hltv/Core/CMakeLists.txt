project(OGS-core)

# Output name
set(MODULE_OUTPUT_FILENAME Core)

# Build a list of headers
file(GLOB PROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# Build a list of sources
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Build as dynamic library
add_library(${MODULE_OUTPUT_FILENAME} MODULE ${PROJECT_HEADERS} ${PROJECT_SOURCES})
project(OGS-tier1)

# Target name
set(TARGET_TIER1 tier1)

# Build a list of headers
file(GLOB PROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Build a list of sources
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Build as static library
add_library(${TARGET_TIER1} STATIC ${PROJECT_HEADERS} ${PROJECT_SOURCES})
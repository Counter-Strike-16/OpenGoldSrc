project(OGS-particleman)

# Build a list of headers
file(GLOB PROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Build a list of sources
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Build as dynamic library
add_library(${PROJECT_NAME} MODULE ${PROJECT_HEADERS} ${PROJECT_SOURCES})
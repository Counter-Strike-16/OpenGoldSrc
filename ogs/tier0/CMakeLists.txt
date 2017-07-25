project(OGS-tier0)

# Build a list of headers
file(GLOB PROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Build a list of sources
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# External sources list
file(GLOB EXTERNAL_SOURCES
	${GOLDSRC_SDK_DIR}/public/tier0/*.cpp
)

# Append external sources to the main sources list
list(APPEND PROJECT_SOURCES ${EXTERNAL_SOURCES})

# Build as dynamic library
add_library(${PROJECT_NAME} MODULE ${PROJECT_HEADERS} ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "tier0"
)
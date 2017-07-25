project(ogs-common)

# List of sources
file(GLOB COMMON_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

if(COMMON_SOURCES)
	# Build as static library
	add_library(${PROJECT_NAME} STATIC ${COMMON_SOURCES})
endif()
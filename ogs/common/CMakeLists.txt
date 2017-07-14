project(ogs-common)

file(GLOB COMMON_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${COMMON_SOURCES})
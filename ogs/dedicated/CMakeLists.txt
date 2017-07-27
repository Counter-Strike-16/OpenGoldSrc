project(OGS-dedicated)

# Add include directories
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Build a list of headers
file(GLOB PROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# Build a list of sources
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

if(WIN32)
	set(TARGET_PLATFORM windows)
elseif(UNIX)
	set(TARGET_PLATFORM linux)
endif()

# Build a list of platform-specific sources
file(GLOB PROJECT_PLATFORM_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET_PLATFORM}/*.cpp
)

# Add platform-specific sources
list(APPEND PROJECT_SOURCES ${PROJECT_PLATFORM_SOURCES})

# External sources list
set(PROJECT_SOURCES_EXTERNAL
	#${CMAKE_CURRENT_SOURCE_DIR}/../engine/ac96/memory/mem.cpp
)

# Add the external sources list to the main list
list(APPEND PROJECT_SOURCES ${PROJECT_SOURCES_EXTERNAL})

if(PROJECT_SOURCES)
	# Build as executable
	add_executable(${PROJECT_NAME} ${PROJECT_HEADERS} ${PROJECT_SOURCES})
	
	# Set properties
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "hlds" # ogsds
	)
	
	# Link with other libs
	#target_link_libraries(${PROJECT_NAME} ogs-common ogs-tier1)
endif()
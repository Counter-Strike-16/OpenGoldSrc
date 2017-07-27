project(OGS-engine)

# Build info config file
#configure_file(BuildInfoConfig.in BuildInfoConfig.hpp)

# Enable SSE4.1 support ([Sh1ft]: I think it should be lowered)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")

# We're building the engine itself
add_definitions(-DREHLDS_SELF)

# Add include directories
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
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

# TODO: revisit

# Build a list of platform-specific sources
file(GLOB_RECURSE SOURCES_TO_EXCLUDE
	${CMAKE_CURRENT_SOURCE_DIR}/impl/*.cpp
)

# Remove all platform-specific sources from the sources list
LIST(REMOVE_ITEM PROJECT_SOURCES
	${SOURCES_TO_EXCLUDE}
)

# Detect current platform
if(WIN32)
	set(PROJECT_TARGET_PLATFORM win)
elseif(UNIX)
	set(PROJECT_TARGET_PLATFORM linux) # unix?
else()
	message(FATAL_ERROR "Unsupported platform!") # or set at null impl?
endif()

# Build current platform sources list
file(GLOB_RECURSE PLATFORM_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/impl/${PROJECT_TARGET_PLATFORM}/*.cpp
)

# Additional files out of the engine path that should be compiled in
set(PROJECT_SOURCES_EXTERNAL
	#crc32.cpp
	#sys_shared.cpp
	${CMAKE_SOURCE_DIR}/ogs/public/registry.cpp
)

# Add external sources to the main sources list
list(APPEND PROJECT_SOURCES ${PROJECT_SOURCES_EXTERNAL})

# Check the main sources list for duplicated entries
list(REMOVE_DUPLICATES PROJECT_SOURCES)

if(PROJECT_SOURCES)
	# Build as dynamic library
	add_library(${PROJECT_NAME} MODULE ${PROJECT_HEADERS} ${PLATFORM_SOURCES} ${PROJECT_SOURCES})

	# Set target properties
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "engine" # hw?
	)

	# Link with additional libraries
	target_link_libraries(${PROJECT_NAME} ogs-tier1)
endif()
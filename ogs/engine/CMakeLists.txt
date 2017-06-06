project(OGS-engine)

# Target name
set(PROJECT_TARGET_ENGINE engine)

# Available options
option(OGS_ENGINE_STATIC "Build the engine as a static library" OFF)

# Detect the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
        FORCE
	)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		None Debug Release RelWithDebInfo MinSizeRel
	)
endif()

# Detect the impl type
if(NOT OGS_ENGINE_IMPL_TYPE)
    set(OGS_ENGINE_IMPL_TYPE Null CACHE STRING
        "Choose the type of engine implementation, options are: Null AC96 RW17."
        FORCE
	)
    set_property(CACHE OGS_ENGINE_IMPL_TYPE PROPERTY STRINGS
		Null AC96 RW17
	)
endif()

# List of sources
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# List of headers
file(GLOB PROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Add include directories
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/${OGS_ENGINE_IMPL_TYPE}
)

string(TOLOWER ${OGS_ENGINE_IMPL_TYPE} OGS_ENGINE_IMPL_TYPE)
add_subdirectory(${OGS_ENGINE_IMPL_TYPE})

# Detect output lib type (static/dynamic)
if(OGS_ENGINE_STATIC)
	set(LIB_TYPE STATIC)
else()
	set(LIB_TYPE MODULE)
endif()

# Build as static/dynamic library
add_library(${PROJECT_TARGET_ENGINE} ${LIB_TYPE} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

message(STATUS "Engine: " ${OGS_ENGINE_IMPL_TYPE} " (" ${LIB_TYPE} ")" \n
	"    - Subsystem Impl: " ${OGS_ENGINE_SUBSYSTEM_IMPL_TYPE}
)

# Set target properties
set_target_properties(${PROJECT_TARGET_ENGINE} PROPERTIES
#	DEBUG_POSTFIX "_d"
	OUTPUT_NAME engine
)

if(NOT ${LIB_TYPE} EQUAL "STATIC")
	set_target_properties(${PROJECT_TARGET_ENGINE} PROPERTIES
		PREFIX ""
	)
endif()
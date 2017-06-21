#
#Launcher executable
#

set( PROJECT_NAME hl )

#Add in the directories
add_sources(
	${SRC_DIR}/common/CCommandLine.cpp
	${SRC_DIR}/common/CCommandLine.h
	${SRC_DIR}/common/FilePaths.cpp
	${SRC_DIR}/common/FilePaths.h
	${SRC_DIR}/common/ICommandLine.h
	${SRC_DIR}/public/engine_launcher_api.h
	${SRC_DIR}/public/interface.cpp
	${SRC_DIR}/public/interface.h
	${SRC_DIR}/public/IRegistry.h
	${SRC_DIR}/public/registry.cpp
	${SRC_DIR}/public/registry.h
	launcher.cpp
)

preprocess_sources()

add_executable( ${PROJECT_NAME} ${PREP_SRCS} )

target_include_directories( ${PROJECT_NAME} PRIVATE
	${SRC_DIR}/common
	${SRC_DIR}/public
	${SHARED_INCLUDE_PATHS}
	${SHARED_EXTERNAL_INCLUDE_PATHS}
)

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${SHARED_DEFS}
)

#Find dependencies
set( PROJECT_LIBS )
if( WIN32 )
	set( PROJECT_LIBS ${PROJECT_LIBS} wsock32 ws2_32 )
endif()

#Link with dependencies
target_link_libraries( ${PROJECT_NAME} 
	"${SHARED_DEPENDENCIES}"
	${PROJECT_LIBS}
)

#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
set_target_properties( ${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${ENGINE_BASE_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ENGINE_BASE_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ENGINE_BASE_PATH}"
)

set( PROJECT_LINK_FLAGS )

if( NOT MSVC AND NOT APPLE )
	#Generate a map file that provides information about the linking stage.
	set( PROJECT_LINK_FLAGS
		${PROJECT_LINK_FLAGS} "-Wl,-Map,${PROJECT_NAME}_map.txt "
	)
endif()

set_target_properties( ${PROJECT_NAME} PROPERTIES
	COMPILE_FLAGS "${LINUX_32BIT_FLAG}"
	LINK_FLAGS "${SHARED_ENGINE_LINKER_FLAGS} ${PROJECT_LINK_FLAGS} ${LINUX_32BIT_FLAG}"
)

#No lib prefix
SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES PREFIX "" )

#Create filters
create_source_groups( "${CMAKE_SOURCE_DIR}" )

clear_sources()

#
#End library
#
#
#	FileSystem library
#	Shared library providing the SteamPipe filesystem. Implements the GoldSource IFileSystem interface.
#

set( PROJECT_NAME filesystem_stdio )

#TODO: stringutils.cpp should be removed. - Solokiller
add_sources(
	${SRC_DIR}/common/StringUtils.cpp
	${SRC_DIR}/public/characterset.cpp
	${SRC_DIR}/public/characterset.h
	${SRC_DIR}/public/interface.cpp
	${SRC_DIR}/public/interface.h
	CFileHandle.cpp
	CFileHandle.h
	CFileSystem.cpp
	CFileSystem.obsolete.cpp
	CFileSystem.h
	CPackFileEntry.h
	CSearchPath.h
	PackFile.h
)

preprocess_sources()

add_library( ${PROJECT_NAME} SHARED ${PREP_SRCS} )

#TODO: common is not used by non-temporary code - Solokiller
target_include_directories( ${PROJECT_NAME} PRIVATE
	${SRC_DIR}/common
	${SRC_DIR}/public
	${SRC_DIR}/public/tier1
	${SHARED_INCLUDE_PATHS}
	${SHARED_EXTERNAL_INCLUDE_PATHS}
)

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${SHARED_DEFS}
)

#Link with dependencies
target_link_libraries( ${PROJECT_NAME} 
	${UNIX_FS_LIB}
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
set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )

#Create filters
create_source_groups( "${CMAKE_SOURCE_DIR}" )

clear_sources()

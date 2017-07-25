###################################################
#                                                 #
#                                                 #
#   Tier0 CMake build file                        #
#                                                 #
#                                                 #
###################################################

set( PROJECT_NAME tier0 )

add_sources(
	assert_dialog.cpp
	cpu.cpp
	#cpu_posix.cpp
	dbg.cpp
	fasttimer.cpp
	mem.cpp
	memblockhdr.cpp
	memstd.cpp
	memvalidate.cpp
	mem_helpers.cpp
	minidump.cpp
	pch_tier0.cpp
	platform.cpp
	#platform_posix.cpp
	pme.cpp
	#pme_posix.cpp
	testthread.cpp
	thread.cpp
	threadtools.cpp
	tier0.cpp
	validator.cpp
	valobject.cpp
	#vcrmode_posix.cpp
	vprof.cpp
)

add_subdirectory( ${SRC_DIR}/public/tier0 public/tier0 )

preprocess_sources()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa " )

add_library( ${PROJECT_NAME} SHARED ${PREP_SRCS} )

target_include_directories( ${PROJECT_NAME} PRIVATE
	${SRC_DIR}/public
	${SRC_DIR}/public/tier0
	${SRC_DIR}/public/tier1
	${SRC_DIR}/common
	${SRC_DIR}/public
	${SRC_DIR}
)

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${SHARED_DEFS}
	TIER0_DLL_EXPORT
	NO_VCR
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

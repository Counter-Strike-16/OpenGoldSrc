project(OGS-filesystem)

# Mostly for common IFileSystem interface
include_directories(
	${CMAKE_SOURCE_DIR}/../hlsdk-core
)

# Detect the impl type
if(NOT OGS_FILESYSTEM_IMPL_TYPE)
    set(OGS_FILESYSTEM_IMPL_TYPE Null CACHE STRING
        "Choose the type of engine implementation, options are: Null Stdio Steam Nar."
        FORCE
	)
    set_property(CACHE OGS_FILESYSTEM_IMPL_TYPE PROPERTY STRINGS
		Null	# Null impl (just for test)
		Stdio	# Standard impl (default) - abstraction level above std + some OS specific code (+ .gcf support since SteamPipe?)
		Steam	# Integration with steam services or something
		Nar		# CSO .nar
	)
endif()

string(TOLOWER ${OGS_FILESYSTEM_IMPL_TYPE} OGS_FILESYSTEM_IMPL_TYPE)
add_subdirectory(${OGS_FILESYSTEM_IMPL_TYPE})

#add_subdirectory( FileSystem_Stdio )
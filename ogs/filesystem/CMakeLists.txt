project(OGS-filesystem)

# Detect the impl type
if(NOT OGS_FILESYSTEM_IMPL_TYPE)
    set(OGS_FILESYSTEM_IMPL_TYPE Null CACHE STRING
        "Choose the type of filesystem module implementation, options are: Null Stdio Steam Nar."
        FORCE
	)
    set_property(CACHE OGS_FILESYSTEM_IMPL_TYPE PROPERTY STRINGS
		Null      # Null impl (just for test)
		Stdio     # Standard impl (default) - abstraction level above std + some OS specific code (+ .gcf support since SteamPipe?)
		#Steam    # Integration with steam services or something
		#Nar      # CSO .nar
		Stdio-GSR # Stdio impl from GoldSrcRebuild
	)
endif()

string(TOLOWER ${OGS_FILESYSTEM_IMPL_TYPE} OGS_FILESYSTEM_IMPL_TYPE)
add_subdirectory(${OGS_FILESYSTEM_IMPL_TYPE})

#add_subdirectory( FileSystem_Stdio )

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "filesystem_${OGS_FILESYSTEM_IMPL_TYPE}"
)
project(OGS-render)

# Detect the impl type
if(NOT OGS_RENDER_IMPL_TYPE)
    set(OGS_RENDER_IMPL_TYPE Null CACHE STRING
        "Choose the type of engine implementation, options are: Null GL Software."
        FORCE
	)
    set_property(CACHE OGS_RENDER_IMPL_TYPE PROPERTY STRINGS
		Null GL Software
	)
endif()

string(TOLOWER ${OGS_RENDER_IMPL_TYPE} OGS_RENDER_IMPL_TYPE)
add_subdirectory(${OGS_RENDER_IMPL_TYPE})
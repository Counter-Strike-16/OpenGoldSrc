project(OGS-gameui)

# Detect the impl type
if(NOT OGS_GAMEUI_IMPL_TYPE)
    set(OGS_GAMEUI_IMPL_TYPE Null CACHE STRING
        "Choose the type of GameUI module implementation, options are: Null Default."
        FORCE
	)
    set_property(CACHE OGS_GAMEUI_IMPL_TYPE PROPERTY STRINGS
		Null    # Null impl (just for test)
		Default # Default implementation
	)
endif()

string(TOLOWER ${OGS_GAMEUI_IMPL_TYPE} OGS_GAMEUI_IMPL_TYPE)
add_subdirectory(${OGS_GAMEUI_IMPL_TYPE})

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "GameUI"
)
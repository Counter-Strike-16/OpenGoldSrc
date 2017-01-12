cmake_minimum_required(VERSION 2.8)
project(OGS-root)

# Set C++ code standard
set(CMAKE_CXX_STANDARD 11) #03
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(
	${CMAKE_SOURCE_DIR}/../hlsdk-core
)

# Add our modules
#add_subdirectory(core)
add_subdirectory(dbg)
add_subdirectory(dedicated)
#add_subdirectory(demoplayer)
add_subdirectory(engine)
add_subdirectory(filesystem)
#add_subdirectory(game)
add_subdirectory(gameui)
add_subdirectory(launcher)
#add_subdirectory(particleman)
add_subdirectory(tier0)
add_subdirectory(tier1)
add_subdirectory(vgui)
add_subdirectory(vgui2)
#add_subdirectory(voice)
add_subdirectory(vstdlib)
project(OGS-root)

# Set C++ code standard
set(CMAKE_CXX_STANDARD 11) #03/14/17 ?
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/../hlsdk-core
	${CMAKE_SOURCE_DIR}/../hlsdk-core/public
	${CMAKE_SOURCE_DIR}/hlsdk
	${CMAKE_SOURCE_DIR}/../shiftutil
	${CMAKE_SOURCE_DIR}/shiftutil
	${CMAKE_CURRENT_SOURCE_DIR}/common
	${CMAKE_CURRENT_SOURCE_DIR}/public
	${CMAKE_SOURCE_DIR}/../goldsrc-sdk/goldsrc/common
	${CMAKE_SOURCE_DIR}/../goldsrc-sdk/goldsrc/public
	${CMAKE_SOURCE_DIR}/../goldsrc-sdk/goldsrc/public/tier1/
)

# Add our modules
##add_subdirectory(dbg)
add_subdirectory(tier1)
add_subdirectory(dedicated)
#add_subdirectory(dedicated-legacy)
#add_subdirectory(engine)
##add_subdirectory(filesystem)
#add_subdirectory(game)
##add_subdirectory(gameui)
add_subdirectory(hltv)
add_subdirectory(hookers)
add_subdirectory(launcher)
#add_subdirectory(particleman)
add_subdirectory(rehlds)
#add_subdirectory(render)
#add_subdirectory(sound)
#add_subdirectory(testsuite)
add_subdirectory(tier0)
#add_subdirectory(unittests)
add_subdirectory(vgui)
##add_subdirectory(vgui2)
#add_subdirectory(voice)
add_subdirectory(vstdlib)
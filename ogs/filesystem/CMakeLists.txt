cmake_minimum_required(VERSION 2.8) # <- TODO: detect absolute minimum
project(OGS-filesystem)

# Mostly for common IFileSystem interface
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../hlsdk-base/)

# TODO: conditional compilation
add_subdirectory(null)		# Null impl (just for test)
add_subdirectory(stdio)		# Standard impl (default) - abstraction level above std + some OS specific code (+ .gcf support since SteamPipe?)
#add_subdirectory(steam)	# Integration with steam services or something
#add_subdirectory(nar)		# CSO .nar
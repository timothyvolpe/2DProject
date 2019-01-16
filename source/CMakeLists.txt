cmake_minimum_required( VERSION 3.0 )

set( ProjectName "2DProject" )

project( ${ProjectName} )

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELWITHDEBINFO
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

# Definitons
ADD_DEFINITIONS( -DUNICODE )
ADD_DEFINITIONS( -D_UNICODE )
ADD_DEFINITIONS( -DWIN32 )
ADD_DEFINITIONS( -DGLEW_STATIC )
ADD_DEFINITIONS( -DSDL_MAIN_HANDLED )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )
ADD_DEFINITIONS( -D_SCL_SECURE_NO_WARNINGS )

# Include and Source Files
file( GLOB Project_INC "${PROJECT_SOURCE_DIR}/_include/*.h" )
file( GLOB Project_INC_BLOCKS "${PROJECT_SOURCE_DIR}/_include/blocks/*.h" )
file( GLOB Project_INC_INTERFACE "${PROJECT_SOURCE_DIR}/_include/interface/*.h" )
file( GLOB Project_INC_SHADER "${PROJECT_SOURCE_DIR}/_include/shader/*.h" )
file( GLOB Project_INC_WORLD "${PROJECT_SOURCE_DIR}/_include/world/*.h" )
file( GLOB Project_INC_WORLD_ENT "${PROJECT_SOURCE_DIR}/_include/world/entities/*.h" )
file( GLOB Project_INC_WORLD_TERG "${PROJECT_SOURCE_DIR}/_include/world/terraingen/*.h" )
file( GLOB Project_INC_SCRIPT "${PROJECT_SOURCE_DIR}/_include/script/*.h" )

file( GLOB Project_SRC "${PROJECT_SOURCE_DIR}/*.cpp" )
file( GLOB Project_SRC_BLOCKS "${PROJECT_SOURCE_DIR}/blocks/*.cpp" )
file( GLOB Project_SRC_INTERFACE "${PROJECT_SOURCE_DIR}/interface/*.cpp" )
file( GLOB Project_SRC_SHADER "${PROJECT_SOURCE_DIR}/shader/*.cpp" )
file( GLOB Project_SRC_WORLD "${PROJECT_SOURCE_DIR}/world/*.cpp" )
file( GLOB Project_SRC_WORLD_ENT "${PROJECT_SOURCE_DIR}/world/entities/*.cpp" )
file( GLOB Project_SRC_WORLD_TERG "${PROJECT_SOURCE_DIR}/world/terraingen/*.cpp" )
file( GLOB Project_SRC_SCRIPT "${PROJECT_SOURCE_DIR}/script/*.cpp" )

include_directories( "${PROJECT_SOURCE_DIR}/_include" )

source_group( "Header Files\\blocks" FILES ${Project_INC_BLOCKS} )
source_group( "Header Files\\interface" FILES ${Project_INC_INTERFACE} )
source_group( "Header Files\\shader" FILES ${Project_INC_SHADER} )
source_group( "Header Files\\world" FILES ${Project_INC_WORLD} )
source_group( "Header Files\\world\\entities" FILES ${Project_INC_WORLD_ENT} )
source_group( "Header Files\\world\\terraingen" FILES ${Project_INC_WORLD_TERG} )
source_group( "Header Files\\script" FILES ${Project_INC_SCRIPT} )

source_group( "Source Files\\blocks" FILES ${Project_SRC_BLOCKS} )
source_group( "Source Files\\interface" FILES ${Project_SRC_INTERFACE} )
source_group( "Source Files\\shader" FILES ${Project_SRC_SHADER} )
source_group( "Source Files\\world" FILES ${Project_SRC_WORLD} )
source_group( "Source Files\\world\\entities" FILES ${Project_SRC_WORLD_ENT} )
source_group( "Source Files\\world\\terraingen" FILES ${Project_SRC_WORLD_TERG} )
source_group( "Source Files\\script" FILES ${Project_SRC_SCRIPT} )

# Boost
set( BOOST_INCLUDE_DIR "NOTBOOST_INCLUDE_DIR-NOTFOUND" CACHE PATH "" )
set( BOOST_LIBRARY_DIR "BOOST_INCLUDE_DIR-NOTFOUND" CACHE PATH "" )

if( EXISTS ${BOOST_INCLUDE_DIR} )
	include_directories( ${BOOST_INCLUDE_DIR} )
else()
	message( FATAL_ERROR "Invalid BOOST_INCLUDE_DIR." )
endif()

if( EXISTS ${BOOST_LIBRARY_DIR} )
	link_directories( ${BOOST_LIBRARY_DIR} )
else()
	message( FATAL_ERROR "Invalid BOOST_LIBRARY_DIR." )
endif()


add_executable( ${ProjectName} WIN32  
	${Project_INC} ${Project_INC_BLOCKS} ${Project_INC_INTERFACE} ${Project_INC_SHADER} ${Project_INC_WORLD} ${Project_INC_WORLD_ENT} ${Project_INC_WORLD_TERG} ${Project_INC_SCRIPT}
	${Project_SRC} ${Project_SRC_BLOCKS} ${Project_SRC_INTERFACE} ${Project_SRC_SHADER} ${Project_SRC_WORLD} ${Project_SRC_WORLD_ENT} ${Project_SRC_WORLD_TERG} ${Project_SRC_SCRIPT} )

if( MSVC )
	set_target_properties( ${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin" )
	set_target_properties( ${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/../bin" )
	set_target_properties( ${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/../bin" )
	
	set_target_properties( ${ProjectName} PROPERTIES DEBUG_POSTFIX "D" )
	set_target_properties( ${ProjectName} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/../build" )
	
	if(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
		message( "Be sure to change the Debugger Working Directory in the generated solution to point to the build folder!" )
	endif()
endif()

# Third Party Libraries

# OpenGL

find_package( OpenGL REQUIRED )
if( OPENGL_FOUND )
	include_directories( ${OPENGL_INCLUDE_DIR} )
	target_link_libraries( ${ProjectName} ${OPENGL_LIBRARIES} )
endif()

# SDL (2.0.3)

set( SDL2_INCLUDE_DIR "SDL2_INCLUDE_DIR-NOTFOUND" CACHE PATH "" )
set( SDL2_LIBRARY "SDL2_LIBRARY-NOTFOUND" CACHE PATH "" )
set( SDL2_LIBRARY_DEBUG "SDL2_LIBRARY_DEBUG-OPTIONAL-NOTFOUND" CACHE PATH "" )
set( SDL2_MAIN "SDL2_MAIN-NOTFOUND" CACHE PATH "" )
set( SDL2_MAIN_DEBUG "SDL2_MAIN_DEBUG-OPTIONAL-NOTFOUND" CACHE PATH "" )
if( EXISTS ${SDL2_INCLUDE_DIR} )
	include_directories( ${SDL2_INCLUDE_DIR} )
else()
	message( FATAL_ERROR "Invalid SDL2_INCLUDE_DIR." )
endif()
if( EXISTS ${SDL2_LIBRARY} )
	if( EXISTS ${SDL2_LIBRARY_DEBUG} )
		target_link_libraries( ${ProjectName} optimized ${SDL2_LIBRARY} debug ${SDL2_LIBRARY_DEBUG} )
	else()
		target_link_libraries( ${ProjectName} ${SDL2_LIBRARY} )
	endif()
else()
	message( FATAL_ERROR "Invalid SDL2_LIBRARY file" )
endif()
if( EXISTS ${SDL2_MAIN} )
	if( EXISTS ${SDL2_MAIN_DEBUG} )
		target_link_libraries( ${ProjectName} optimized ${SDL2_MAIN} debug ${SDL2_MAIN_DEBUG} )
	else()
		target_link_libraries( ${ProjectName} ${SDL2_MAIN} )
	endif()
else()
	message( FATAL_ERROR "Invalid SDL2_MAIN file" )
endif()


# GLEW
set( GLEW_INCLUDE_DIR "GLEW_INCLUDE_DIR-NOTFOUND" CACHE PATH "" )
set( GLEW_LIBRARY "GLEW_LIBRARY-NOTFOUND" CACHE FILEPATH "" )
set( GLEW_LIBRARY_DEBUG "GLEW_LIBRARY_DEBUG-OPTIONAL-NOTFOUND" CACHE FILEPATH "" )
if( EXISTS ${GLEW_INCLUDE_DIR} )
	include_directories( ${GLEW_INCLUDE_DIR} )
else()
	message( FATAL_ERROR "Invalid GLEW_INCLUDE_DIR." )
endif()
if( EXISTS ${GLEW_LIBRARY} )
	if( EXISTS ${GLEW_LIBRARY_DEBUG} )
		target_link_libraries( ${ProjectName} optimized ${GLEW_LIBRARY} debug ${GLEW_LIBRARY_DEBUG} )
	else()
		target_link_libraries( ${ProjectName} ${GLEW_LIBRARY} )
	endif()
else()
	message( FATAL_ERROR "Invalid GLEW_LIBRARY file" )
endif()

# GLM
set( GLM_INCLUDE_DIR "GLM_INCLUDE_DIR-NOTFOUND" CACHE PATH "" )
if( EXISTS ${GLM_INCLUDE_DIR} )
	include_directories( ${GLM_INCLUDE_DIR} )
else()
	message( FATAL_ERROR "Invalid GLM_INCLUDE_DIR" )
endif()

# zlib

set( ZLIB_LIBRARY "ZLIB_LIBRARY-NOTFOUND" CACHE PATH "" )
set( ZLIB_LIBRARY_DEBUG "ZLIB_LIBRARY_DEBUG-OPTIONAL-NOTFOUND" CACHE PATH "" )
if( EXISTS ${ZLIB_LIBRARY} )
	if( EXISTS ${ZLIB_LIBRARY_DEBUG} )
		target_link_libraries( ${ProjectName} optimized ${ZLIB_LIBRARY} debug ${ZLIB_LIBRARY_DEBUG}  )
	else()
		target_link_libraries( ${ProjectName} ${ZLIB_LIBRARY} )
	endif()
else()
	message( FATAL_ERROR "Invalid ZLIB_LIBRARY file" )
endif()

# libpng
set( LIBPNG_INCLUDE "LIBPNG_INCLUDE-NOTFOUND" CACHE PATH "" )
set( LIBPNG_LIBRARY "LIBPNG_LIBRARY-NOTFOUND" CACHE PATH "" )
set( LIBPNG_LIBARY_DEBUG "LIBPNG_LIBRARY_DEBUG-OPTIONAL-NOTFOUND" CACHE PATH "" )
if( EXISTS ${LIBPNG_INCLUDE} )
	include_directories( ${LIBPNG_INCLUDE} )
else()
	message( FATAL_ERROR "Invalid LIBPNG_INCLUDE." )
endif()
if( EXISTS ${LIBPNG_LIBRARY} )
	if( EXISTS ${LIBPNG_LIBARY_DEBUG} )
		target_link_libraries( ${ProjectName} optimized ${LIBPNG_LIBRARY} debug ${LIBPNG_LIBARY_DEBUG} )
	else()
		target_link_libraries( ${ProjectName} ${LIBPNG_LIBRARY} )
	endif()
else()
	message( FATAL_ERROR "Invalid LIBPNG_LIBRARY file" )
endif()

# E:\C++ Lib Folder\Box2D\Build\Box2D\Debug

# Box2D
set( BOX2D_INCLUDE "BOX2D_INCLUDE-NOTFOUND" CACHE PATH "" )
set( BOX2D_LIBRARY "BOX2D_LIBRARY-NOTFOUND" CACHE PATH "" )
set( BOX2D_LIBRARY_DEBUG "BOX2D_LIBRARY_DEBUG-NOTFOUND" CACHE PATH "" )
if( EXISTS ${BOX2D_INCLUDE} )
	include_directories( ${BOX2D_INCLUDE} )
else()
	message( FATAL_ERROR "Invalid BOX2D_INCLUDE." )
endif()
if( EXISTS ${BOX2D_LIBRARY} )
	if( EXISTS ${BOX2D_LIBRARY_DEBUG} )
		target_link_libraries( ${ProjectName} optimized ${BOX2D_LIBRARY} debug ${BOX2D_LIBRARY_DEBUG} )
		target_link_libraries( ${ProjectName} )
	else()
		target_link_libraries( ${ProjectName} ${BOX2D_LIBRARY} )
	endif()
else()
	message( FATAL_ERROR "Invalid BOX2D_LIBRARY file" )
endif()

# freetype

set( FREETYPE_INCLUDE "FREETYPE_INCLUDE-NOTFOUND" CACHE PATH "" )
set( FREETYPE_LIBRARY "FREETYPE_LIBRARY-NOTFOUND" CACHE PATH "" )
set( FREETYPE_LIBRARY_DEBUG "FREETYPE_LIBRARY_DEBUG-NOTFOUND" CACHE PATH "" )
if( EXISTS ${FREETYPE_INCLUDE} )
	include_directories( ${FREETYPE_INCLUDE} )
else()
	message( FATAL_ERROR "Invalid FREETYPE_INCLUDE." )
endif()
if( EXISTS ${FREETYPE_LIBRARY} )
	if( EXISTS ${FREETYPE_LIBRARY_DEBUG} )
		target_link_libraries( ${ProjectName} optimized ${FREETYPE_LIBRARY} debug ${FREETYPE_LIBRARY_DEBUG} )
		target_link_libraries( ${ProjectName} )
	else()
		target_link_libraries( ${ProjectName} ${FREETYPE_LIBRARY} )
	endif()
else()
	message( FATAL_ERROR "Invalid FREETYPE_LIBRARY file" )
endif()

# lua

set( LUA_INCLUDE "LUA_INCLUDE-NOTFOUND" CACHE PATH "" )
set( LUA_LIBRARY "LUA_LIBRARY-NOTFOUND" CACHE PATH "" )
if( EXISTS ${LUA_INCLUDE} )
	include_directories( ${LUA_INCLUDE} )
else()
	message( FATAL_ERROR "Invalid LUA_INCLUDE." )
endif()
if( EXISTS ${LUA_LIBRARY} )
	if( EXISTS ${LUA_LIBRARY} )
		target_link_libraries( ${ProjectName} ${LUA_LIBRARY} )
		target_link_libraries( ${ProjectName} )
	else()
		target_link_libraries( ${ProjectName} ${LUA_LIBRARY} )
	endif()
else()
	message( FATAL_ERROR "Invalid LUA_LIBRARY file" )
endif()

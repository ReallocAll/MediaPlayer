cmake_minimum_required(VERSION 3.21)

project(MediaPlayer)
project(convert)

set(CMAKE_C_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SRC_FILES_DIR
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/include/libspng/*.c
    ${PROJECT_SOURCE_DIR}/include/miniz/*.c
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

add_definitions(
    -D"_CRT_SECURE_NO_WARNINGS"
    -DPLUGIN_VERSION_UNDEFINED
    -DWIN32_LEAN_AND_MEAN
)

add_compile_options(
    /diagnostics:column
    /FC
    /GL
    /nologo
    /permissive-
    /sdl
    /utf-8
    /Zi
)

add_link_options(
    /LTCG
    /INCREMENTAL:NO
)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD")

add_library(MediaPlayer SHARED ${SRC_FILES_DIR})

add_executable(convert ${PROJECT_SOURCE_DIR}/tools/convert.c)

set_target_properties(MediaPlayer PROPERTIES OUTPUT_NAME "MediaPlayer")

target_link_libraries(MediaPlayer 
	"${CMAKE_SOURCE_DIR}/lib/littlehooker.lib"
	"${CMAKE_SOURCE_DIR}/lib/minhook.x64.lib"
	"${CMAKE_SOURCE_DIR}/lib/nbsparser.lib"
	"${CMAKE_SOURCE_DIR}/lib/libuv.lib"
	"ws2_32.lib"
	"Iphlpapi.lib"
	"dbghelp.lib"
	"userenv.lib"
)

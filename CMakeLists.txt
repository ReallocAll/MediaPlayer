cmake_minimum_required(VERSION 3.21)

project(MediaPlayer)

set(CMAKE_C_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SRC_FILES_DIR
    ${PROJECT_SOURCE_DIR}/src/*.c
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

add_definitions(
    -D"NDEBUG"
    -D"NOMINMAX"
    -D"TEMPLATE_EXPORTS"
    -D"UNICODE"
    -D"WIN32_LEAN_AND_MEAN"
    -D"_AMD64_"
    -D"_CRT_SECURE_NO_WARNINGS"
    -D"_UNICODE"
    -D"_USRDLL"
    -D"_WINDLL"
    -D"_WINDOWS"
)

add_compile_options(
    /diagnostics:column
    /EHsc
    /FC
    /GL
    /nologo
    /permissive-
    /sdl
    /utf-8
    /Zc:inline
    /Zi
)

add_link_options(
    /LTCG
    /INCREMENTAL:NO
)

set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD")

add_library(MediaPlayer SHARED ${SRC_FILES_DIR})

set_target_properties(MediaPlayer PROPERTIES OUTPUT_NAME "MediaPlayer")

target_link_libraries(MediaPlayer 
	"${CMAKE_SOURCE_DIR}/lib/littlehooker.lib"
	"${CMAKE_SOURCE_DIR}/lib/minhook.x64.lib"
)

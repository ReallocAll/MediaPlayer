cmake_minimum_required(VERSION 3.0)

project(MediaPlayer)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_PREFIX_PATH .)

file(GLOB_RECURSE SRC_FILES_DIR
    ${PROJECT_SOURCE_DIR}/include/libspng/*.c
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/mc/*.c
    ${PROJECT_SOURCE_DIR}/include/nbsparser/*.c
    ${PROJECT_SOURCE_DIR}/include/miniz/*.c
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

add_definitions(
    -DPLUGIN_VERSION_UNDEFINED
    -DPLUGIN_ENVIRONMENT
    -DSPNG_STATIC
)

add_library(MediaPlayer SHARED ${SRC_FILES_DIR})
set_target_properties(MediaPlayer PROPERTIES OUTPUT_NAME "MediaPlayer")

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(
        -D_GNU_SOURCE
    )
    find_package(ModLoader REQUIRED)
    target_link_libraries(MediaPlayer 
        "${CMAKE_SOURCE_DIR}/lib/libserver_modloader.so"
        "${CMAKE_SOURCE_DIR}/lib/libuv.so"
        "${CMAKE_SOURCE_DIR}/lib/libcutils.a"
    )
    install(TARGETS MediaPlayer DESTINATION "/home/ubuntu/bds/mods")
    
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    project(convert)
    add_executable(convert ${PROJECT_SOURCE_DIR}/tools/convert.c)
    add_definitions(
        -D"_CRT_SECURE_NO_WARNINGS"
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
    target_link_libraries(MediaPlayer 
    "${CMAKE_SOURCE_DIR}/lib/libuv.lib"
    "${CMAKE_SOURCE_DIR}/lib/cutils.lib"
    "ws2_32.lib"
    "Iphlpapi.lib"
    "dbghelp.lib"
    "userenv.lib"
    )
endif()

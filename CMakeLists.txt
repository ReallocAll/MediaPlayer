cmake_minimum_required(VERSION 3.29)

project(MediaPlayer LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE SRC_FILES_DIR
    ${PROJECT_SOURCE_DIR}/include/libspng/*.c
    ${PROJECT_SOURCE_DIR}/include/stb/*.c
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/mc/*.c
    ${PROJECT_SOURCE_DIR}/include/nbsparser/*.c
    ${PROJECT_SOURCE_DIR}/include/miniz/*.c
)

add_library(MediaPlayer SHARED ${SRC_FILES_DIR})
set_target_properties(MediaPlayer PROPERTIES OUTPUT_NAME "MediaPlayer")

add_executable(convert ${PROJECT_SOURCE_DIR}/tools/convert.c)
set_target_properties(convert PROPERTIES OUTPUT_NAME "convert")

target_include_directories(MediaPlayer PRIVATE
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/include/libuv
)

target_link_directories(MediaPlayer PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
)

target_compile_definitions(MediaPlayer PRIVATE
    PLUGIN_VERSION_UNDEFINED
    PLUGIN_ENVIRONMENT
    SPNG_STATIC
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_definitions(MediaPlayer PRIVATE
        _GNU_SOURCE
    )
    find_package(ModLoader REQUIRED)
    target_link_libraries(MediaPlayer
        "${CMAKE_SOURCE_DIR}/lib/libserver_modloader.so"
        "${CMAKE_SOURCE_DIR}/lib/libuv.so"
        "${CMAKE_SOURCE_DIR}/lib/libcutils.a"
    )
    install(TARGETS MediaPlayer DESTINATION mods)

elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(MediaPlayer PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
    )
    target_compile_definitions(convert PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
    set_target_properties(MediaPlayer PROPERTIES
        LINK_FLAGS "-Xlinker /NODEFAULTLIB:MSVCRTD"
    )
    target_link_libraries(MediaPlayer
        "${CMAKE_SOURCE_DIR}/lib/libuv.lib"
        "${CMAKE_SOURCE_DIR}/lib/cutils.lib"
        ws2_32
        Iphlpapi
        dbghelp
        userenv
        lightbase
    )
    install(TARGETS MediaPlayer DESTINATION plugins)
endif()

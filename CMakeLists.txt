cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

project(light_object C CXX ASM)

pico_sdk_init()

add_subdirectory(demo)

add_library(light_object INTERFACE)

target_include_directories(
    light_object
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(
    light_object
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/light_object.c
)

if(DEFINED REND_RENDER_DEBUG)
    target_compile_definitions(
        rend
        INTERFACE
        REND_DEBUG_DISPLAY_PIXEL
    )
endif()

if(DEFINED LIGHT_API_TRACE)
    target_compile_definitions(
        light_object
        INTERFACE
        LIGHT_API_TRACE
    )
endif()

target_link_libraries(
    light_object
    INTERFACE
)

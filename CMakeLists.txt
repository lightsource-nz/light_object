cmake_minimum_required(VERSION 3.12)
project(light_object C CXX ASM)

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

target_link_libraries(
    light_object
    INTERFACE
    light_common
)

if(LIGHT_SYSTEM STREQUAL PICO_SDK)
        target_link_libraries(
                light_object
                INTERFACE
                pico_platform
                pico_sync
        )
endif()

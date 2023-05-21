add_executable(light_object_demo)

target_sources(light_object_demo PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/main.c)

target_link_libraries(
        light_object_demo PUBLIC
        light_object
)
if(LIGHT_SYSTEM STREQUAL PICO_SDK)
        if(NOT PICO_NO_HARDWARE)
                pico_enable_stdio_semihosting(light_object_demo 1)
                target_link_libraries(light_object_demo PUBLIC pico_stdlib)
                pico_add_extra_outputs(light_object_demo)
        endif()
endif()

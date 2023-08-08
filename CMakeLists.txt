cmake_minimum_required(VERSION 3.12)

include(cmakes/pico_sdk_import.cmake)

project(sh1107-pico-c CXX C ASM)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    pico_sdk_init()
endif()

add_library(sh1107-pico-c INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/sh1107.c
        ${CMAKE_CURRENT_LIST_DIR}/include/sh1107.h
        )

set_target_properties(sh1107-pico-c PROPERTIES PUBLIC_HEADER
        include/sh1107.h
        )

target_link_libraries(sh1107-pico-c INTERFACE
        hardware_i2c
        )

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_executable(dummy-app test/main.c src/sh1107.c include/font.h src/font.c)
    target_link_libraries(dummy-app pico_stdlib sh1107-pico-c hardware_spi)
    include_directories(dummy-app include/)
    pico_add_extra_outputs(dummy-app)
endif()

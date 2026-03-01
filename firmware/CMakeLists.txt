cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(rp2040_binary_clock C CXX ASM)

pico_sdk_init()

add_executable(rp2040_binary_clock
    main.c
)

target_link_libraries(rp2040_binary_clock 
    pico_stdlib
    hardware_i2c
    hardware_watchdog
)

pico_add_extra_outputs(rp2040_binary_clock)

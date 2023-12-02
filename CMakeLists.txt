cmake_minimum_required(VERSION 3.15)

include(pico_sdk_import.cmake)

project(green_sense_embedded)
set(PICO_BOARD "pico_w")
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(bin)
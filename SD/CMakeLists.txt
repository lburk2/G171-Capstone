cmake_minimum_required(VERSION 3.19)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(SD C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(SD
        main.c
        hw_config.c
        )

add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

target_compile_options(SD PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)

# pull in common dependencies
target_link_libraries(SD         
        pico_stdlib
        FatFs_SPI
        hardware_clocks
        hardware_adc
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(SD)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

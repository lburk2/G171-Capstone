cmake_minimum_required(VERSION 3.19)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(LCD C CXX ASM) 
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(LCD
        main.c
        LCD.c
        LCD.h
        menuButtons.c
        menuButtons.h
        font24.h
        imageN.h
        font.h
        OLED.c
        OLED.h 
        )
#
# pull in common dependencies
target_link_libraries(LCD         
        pico_stdlib
        hardware_spi
        hardware_i2c
        hardware_irq
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(LCD)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

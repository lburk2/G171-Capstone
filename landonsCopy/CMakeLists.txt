cmake_minimum_required(VERSION 3.19)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(SYNTH C CXX ASM) 
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(SYNTH
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
        hw_config.c
        MCP23017.c
        MCP23017.h
        pico_button_matrix.c
        pico_button_matrix.h
        myNCO.c #TAKE OUT IF NOT USE
        myNCO.h
        pwm_audio_setup.c
        pwm_audio_setup.h
        )
#
# pull in common dependencies

add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

target_compile_options(SYNTH PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)

target_link_libraries(SYNTH         
        pico_stdlib
        hardware_spi
        hardware_i2c
        hardware_irq
        FatFs_SPI
        hardware_clocks
        hardware_adc
        hardware_dma
        hardware_pwm
        hardware_sync
        hardware_adc
        hardware_timer
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(SYNTH)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

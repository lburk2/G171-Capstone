cmake_minimum_required(VERSION 3.19)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(evan_ADSR_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    fir_test.c
)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_irq
    hardware_pwm
    hardware_sync
    hardware_gpio
    hardware_adc
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
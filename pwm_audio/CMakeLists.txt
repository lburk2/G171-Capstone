# CMakeLists.txt
#
# Copyright (c) 2022 Daniel Collins
#
# This file is part of rp2040_i2s_example.
#
# rp2040_i2s_example is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License, version 3 as published by the
# Free Software Foundation.
#
# rp2040_i2s_example is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.
#
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# rp2040_i2s_example. If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.13)



set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
# Pull in Raspberry Pi Pico SDK (must be before project)

project(pwm_audio C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pwm_audio 
        #pwm_audio_dma.c
        pwm_audio_setup.c
        pico_keypad4x4.c
        main.c
        )

#target_sources()


# no_flash means the target is to run from RAM
# pico_set_binary_type(i2s_test no_flash)

pico_enable_stdio_uart(pwm_audio 0)
pico_enable_stdio_usb(pwm_audio 1)

# Add the standard library to the build
target_link_libraries(pwm_audio pico_stdlib)

# Add any user requested libraries
target_link_libraries(pwm_audio
        hardware_dma
        hardware_irq
        hardware_pwm
        hardware_sync
        hardware_adc
        hardware_timer
        )

pico_add_extra_outputs(pwm_audio)

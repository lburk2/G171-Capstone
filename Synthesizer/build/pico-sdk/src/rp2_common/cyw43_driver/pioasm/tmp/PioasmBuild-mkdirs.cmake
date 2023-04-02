# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/landonburk/pico/pico-sdk/tools/pioasm"
  "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pioasm"
  "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm"
  "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/tmp"
  "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src"
  "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/landonburk/Documents/G171-Capstone/Synthesizer/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()

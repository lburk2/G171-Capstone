# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/VSARM/sdk/pico/pico-sdk/tools/elf2uf2"
  "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2"
  "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2"
  "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2/tmp"
  "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2/src/ELF2UF2Build-stamp"
  "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2/src"
  "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "Z:/Semester8/Capstone II/Code/G171-Capstone/Synthesizer/build/elf2uf2/src/ELF2UF2Build-stamp${cfgdir}") # cfgdir has leading slash
endif()

# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "Z:\Semester8\Capstone II\Code\G171-Capstone\LCD"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "Z:\Semester8\Capstone II\Code\G171-Capstone\LCD\build"

# Utility rule file for bs2_default_bin.

# Include any custom commands dependencies for this target.
include pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/compiler_depend.make

# Include the progress variables for this target.
include pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/progress.make

pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin: pico-sdk/src/rp2_common/boot_stage2/bs2_default.bin

pico-sdk/src/rp2_common/boot_stage2/bs2_default.bin: pico-sdk/src/rp2_common/boot_stage2/bs2_default.elf
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir="Z:\Semester8\Capstone II\Code\G171-Capstone\LCD\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Generating bs2_default.bin"
	cd /d Z:\SC5W9U~D\CC0CMN~O\Code\GUMGNH~T\LCD\build\pico-sdk\src\R5UI5O~5\BCMI10~0 && "C:\Program FiC\VSARM\armcc\bin\arm-none-eabi-objcopy.exe" -Obinary "Z:/Semester8/Capstone II/Code/G171-Capstone/LCD/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default.elf" "Z:/Semester8/Capstone II/Code/G171-Capstone/LCD/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default.bin"

bs2_default_bin: pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin
bs2_default_bin: pico-sdk/src/rp2_common/boot_stage2/bs2_default.bin
bs2_default_bin: pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/build.make
.PHONY : bs2_default_bin

# Rule to build all files generated by this target.
pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/build: bs2_default_bin
.PHONY : pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/build

pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/clean:
	cd /d Z:\SC5W9U~D\CC0CMN~O\Code\GUMGNH~T\LCD\build\pico-sdk\src\R5UI5O~5\BCMI10~0 && $(CMAKE_COMMAND) -P CMakeFiles\bs2_default_bin.dir\cmake_clean.cmake
.PHONY : pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/clean

pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "Z:\Semester8\Capstone II\Code\G171-Capstone\LCD" C:\VSARM\sdk\pico\pico-sdk\src\rp2_common\boot_stage2 "Z:\Semester8\Capstone II\Code\G171-Capstone\LCD\build" "Z:\Semester8\Capstone II\Code\G171-Capstone\LCD\build\pico-sdk\src\rp2_common\boot_stage2" "Z:\Semester8\Capstone II\Code\G171-Capstone\LCD\build\pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_bin.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default_bin.dir/depend

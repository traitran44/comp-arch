# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.16.4/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.16.4/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ttran/comp-arch/Project1_CPP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ttran/comp-arch/Project1_CPP/build

# Include any dependencies generated for this target.
include CMakeFiles/cachesim.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cachesim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cachesim.dir/flags.make

CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.o: CMakeFiles/cachesim.dir/flags.make
CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.o: ../src/cachesim_driver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ttran/comp-arch/Project1_CPP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.o -c /Users/ttran/comp-arch/Project1_CPP/src/cachesim_driver.cpp

CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ttran/comp-arch/Project1_CPP/src/cachesim_driver.cpp > CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.i

CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ttran/comp-arch/Project1_CPP/src/cachesim_driver.cpp -o CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.s

CMakeFiles/cachesim.dir/src/cache.cpp.o: CMakeFiles/cachesim.dir/flags.make
CMakeFiles/cachesim.dir/src/cache.cpp.o: ../src/cache.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ttran/comp-arch/Project1_CPP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/cachesim.dir/src/cache.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cachesim.dir/src/cache.cpp.o -c /Users/ttran/comp-arch/Project1_CPP/src/cache.cpp

CMakeFiles/cachesim.dir/src/cache.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cachesim.dir/src/cache.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ttran/comp-arch/Project1_CPP/src/cache.cpp > CMakeFiles/cachesim.dir/src/cache.cpp.i

CMakeFiles/cachesim.dir/src/cache.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cachesim.dir/src/cache.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ttran/comp-arch/Project1_CPP/src/cache.cpp -o CMakeFiles/cachesim.dir/src/cache.cpp.s

# Object files for target cachesim
cachesim_OBJECTS = \
"CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.o" \
"CMakeFiles/cachesim.dir/src/cache.cpp.o"

# External object files for target cachesim
cachesim_EXTERNAL_OBJECTS =

cachesim: CMakeFiles/cachesim.dir/src/cachesim_driver.cpp.o
cachesim: CMakeFiles/cachesim.dir/src/cache.cpp.o
cachesim: CMakeFiles/cachesim.dir/build.make
cachesim: CMakeFiles/cachesim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ttran/comp-arch/Project1_CPP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable cachesim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cachesim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cachesim.dir/build: cachesim

.PHONY : CMakeFiles/cachesim.dir/build

CMakeFiles/cachesim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cachesim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cachesim.dir/clean

CMakeFiles/cachesim.dir/depend:
	cd /Users/ttran/comp-arch/Project1_CPP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ttran/comp-arch/Project1_CPP /Users/ttran/comp-arch/Project1_CPP /Users/ttran/comp-arch/Project1_CPP/build /Users/ttran/comp-arch/Project1_CPP/build /Users/ttran/comp-arch/Project1_CPP/build/CMakeFiles/cachesim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cachesim.dir/depend


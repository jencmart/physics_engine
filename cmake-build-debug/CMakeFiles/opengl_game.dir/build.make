# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /opt/clion/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jencmart/personal-projects/opengl-game

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jencmart/personal-projects/opengl-game/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/opengl_game.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/opengl_game.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/opengl_game.dir/flags.make

CMakeFiles/opengl_game.dir/main.cpp.o: CMakeFiles/opengl_game.dir/flags.make
CMakeFiles/opengl_game.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jencmart/personal-projects/opengl-game/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/opengl_game.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/opengl_game.dir/main.cpp.o -c /home/jencmart/personal-projects/opengl-game/main.cpp

CMakeFiles/opengl_game.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opengl_game.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jencmart/personal-projects/opengl-game/main.cpp > CMakeFiles/opengl_game.dir/main.cpp.i

CMakeFiles/opengl_game.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opengl_game.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jencmart/personal-projects/opengl-game/main.cpp -o CMakeFiles/opengl_game.dir/main.cpp.s

# Object files for target opengl_game
opengl_game_OBJECTS = \
"CMakeFiles/opengl_game.dir/main.cpp.o"

# External object files for target opengl_game
opengl_game_EXTERNAL_OBJECTS =

opengl_game: CMakeFiles/opengl_game.dir/main.cpp.o
opengl_game: CMakeFiles/opengl_game.dir/build.make
opengl_game: /usr/lib/x86_64-linux-gnu/libOpenGL.so
opengl_game: /usr/lib/x86_64-linux-gnu/libGLX.so
opengl_game: /usr/lib/x86_64-linux-gnu/libGLU.so
opengl_game: /usr/lib/x86_64-linux-gnu/libGLEW.so
opengl_game: /usr/lib/x86_64-linux-gnu/libglfw.so.3.2
opengl_game: CMakeFiles/opengl_game.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jencmart/personal-projects/opengl-game/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable opengl_game"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opengl_game.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/opengl_game.dir/build: opengl_game

.PHONY : CMakeFiles/opengl_game.dir/build

CMakeFiles/opengl_game.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/opengl_game.dir/cmake_clean.cmake
.PHONY : CMakeFiles/opengl_game.dir/clean

CMakeFiles/opengl_game.dir/depend:
	cd /home/jencmart/personal-projects/opengl-game/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jencmart/personal-projects/opengl-game /home/jencmart/personal-projects/opengl-game /home/jencmart/personal-projects/opengl-game/cmake-build-debug /home/jencmart/personal-projects/opengl-game/cmake-build-debug /home/jencmart/personal-projects/opengl-game/cmake-build-debug/CMakeFiles/opengl_game.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/opengl_game.dir/depend


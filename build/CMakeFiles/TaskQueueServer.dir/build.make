# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/shibbu04/matrecomm_distributed_queue_implementation_task

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/shibbu04/matrecomm_distributed_queue_implementation_task/build

# Include any dependencies generated for this target.
include CMakeFiles/TaskQueueServer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/TaskQueueServer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/TaskQueueServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TaskQueueServer.dir/flags.make

CMakeFiles/TaskQueueServer.dir/src/main.cpp.o: CMakeFiles/TaskQueueServer.dir/flags.make
CMakeFiles/TaskQueueServer.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/TaskQueueServer.dir/src/main.cpp.o: CMakeFiles/TaskQueueServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/shibbu04/matrecomm_distributed_queue_implementation_task/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TaskQueueServer.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TaskQueueServer.dir/src/main.cpp.o -MF CMakeFiles/TaskQueueServer.dir/src/main.cpp.o.d -o CMakeFiles/TaskQueueServer.dir/src/main.cpp.o -c /home/shibbu04/matrecomm_distributed_queue_implementation_task/src/main.cpp

CMakeFiles/TaskQueueServer.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TaskQueueServer.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/shibbu04/matrecomm_distributed_queue_implementation_task/src/main.cpp > CMakeFiles/TaskQueueServer.dir/src/main.cpp.i

CMakeFiles/TaskQueueServer.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TaskQueueServer.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/shibbu04/matrecomm_distributed_queue_implementation_task/src/main.cpp -o CMakeFiles/TaskQueueServer.dir/src/main.cpp.s

# Object files for target TaskQueueServer
TaskQueueServer_OBJECTS = \
"CMakeFiles/TaskQueueServer.dir/src/main.cpp.o"

# External object files for target TaskQueueServer
TaskQueueServer_EXTERNAL_OBJECTS =

TaskQueueServer: CMakeFiles/TaskQueueServer.dir/src/main.cpp.o
TaskQueueServer: CMakeFiles/TaskQueueServer.dir/build.make
TaskQueueServer: libtaskqueue_lib.a
TaskQueueServer: /usr/local/lib/libPocoNet.so.103
TaskQueueServer: /usr/local/lib/libPocoDataPostgreSQL.so.103
TaskQueueServer: /usr/local/lib/libPocoData.so.103
TaskQueueServer: /usr/lib/x86_64-linux-gnu/libpq.so
TaskQueueServer: /usr/lib/x86_64-linux-gnu/libpq.so
TaskQueueServer: /usr/local/lib/libPocoJSON.so.103
TaskQueueServer: /usr/local/lib/libPocoFoundation.so.103
TaskQueueServer: CMakeFiles/TaskQueueServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/shibbu04/matrecomm_distributed_queue_implementation_task/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable TaskQueueServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TaskQueueServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TaskQueueServer.dir/build: TaskQueueServer
.PHONY : CMakeFiles/TaskQueueServer.dir/build

CMakeFiles/TaskQueueServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TaskQueueServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TaskQueueServer.dir/clean

CMakeFiles/TaskQueueServer.dir/depend:
	cd /home/shibbu04/matrecomm_distributed_queue_implementation_task/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/shibbu04/matrecomm_distributed_queue_implementation_task /home/shibbu04/matrecomm_distributed_queue_implementation_task /home/shibbu04/matrecomm_distributed_queue_implementation_task/build /home/shibbu04/matrecomm_distributed_queue_implementation_task/build /home/shibbu04/matrecomm_distributed_queue_implementation_task/build/CMakeFiles/TaskQueueServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TaskQueueServer.dir/depend


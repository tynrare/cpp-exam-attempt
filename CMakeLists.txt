cmake_minimum_required (VERSION 2.8)
project (CppTest)

#version number
set (CppTest_VERSION_MAJOR 1)
set (CppTest_VERSION_MINOR 0)

#include the subdirectory containing our libs
add_subdirectory (Classes)
include_directories(Classes)
#indicate the entry point for the executable
add_executable (CppTest Classes Main.cpp)

# Indicate which libraries to include during the link process.
target_link_libraries (CppTest Classes pthread)

install (TARGETS CppTest DESTINATION bin)

cmake_minimum_required(VERSION 3.14)

project(RootProject)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

add_subdirectory(src)
add_subdirectory(test)
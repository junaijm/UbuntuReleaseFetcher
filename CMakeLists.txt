cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0167 NEW)
cmake_policy(SET CMP0144 NEW)

project(RootProject)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

add_subdirectory(src)
add_subdirectory(test)
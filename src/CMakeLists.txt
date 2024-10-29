cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0167 NEW)
cmake_policy(SET CMP0144 NEW)

project(UbuntuReleaseFetcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

add_executable(UbuntuReleaseFetcher BoostHttpClient.cpp FileLogger.cpp main.cpp UbuntuReleaseFetcher.cpp UbuntuReleaseInfo.cpp)

# Download and extract the boost library from GitHub
message(STATUS "Downloading and extracting boost library sources. This will take some time...")
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE) # Needed to print downloading progress
FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.zip # downloading a zip release speeds up the download
    USES_TERMINAL_DOWNLOAD TRUE 
    GIT_PROGRESS TRUE   
    DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)
target_link_libraries(UbuntuReleaseFetcher PUBLIC Boost::json Boost::beast Boost::program_options)

#find_package(OpenSSL REQUIRED)
#target_link_libraries(UbuntuReleaseFetcher PUBLIC OpenSSL::SSL)

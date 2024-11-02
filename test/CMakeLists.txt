cmake_minimum_required(VERSION 3.14)

project(UbuntuReleaseFetcherTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_COMPILE_WARNING_AS_ERROR ON) - Commented due to some of the Boost library build failure.

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

add_executable(UbuntuReleaseFetcherTest UbuntuReleaseFetcherTest.cpp ../src/UbuntuReleaseFetcher.cpp ../src/UbuntuReleaseInfo.cpp)

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
target_link_libraries(UbuntuReleaseFetcherTest Boost::json)

# Enable Google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()
target_link_libraries(UbuntuReleaseFetcherTest GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(UbuntuReleaseFetcherTest)

# Copy test data to binary directory.
file(COPY testData DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
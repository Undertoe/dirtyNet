project(dirtyNet)

cmake_minimum_required(VERSION 3.23)
include(CTest)

set(CMAKE_CXX_STANDARD 23)
include(FetchContent)

# Header-only Catch2 v2
FetchContent_Declare(
    catch2_v2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.10 
)
FetchContent_MakeAvailable(catch2_v2)
get_target_property(_catch_inc Catch2::Catch2 INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "Catch2 v2 single_include at: ${catch2_v2_SOURCE_DIR}/single_include")

# FetchContent_Declare(
#     googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG v1.17.0 
# )
# FetchContent_MakeAvailable(googletest)

# set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable Google Benchmark tests" FORCE)

# FetchContent_Declare(
#   googlebenchmark
#   GIT_REPOSITORY https://github.com/google/benchmark.git
#   GIT_TAG v1.9.4 
# )
# FetchContent_MakeAvailable(googlebenchmark)

find_package(PkgConfig REQUIRED)
pkg_check_modules(BENCHMARK REQUIRED benchmark)


include(CheckIncludeFileCXX)
check_include_file_cxx("benchmark/benchmark.h" HAVE_BENCHMARK_H)

message(STATUS "Google Benchmark header found: ${HAVE_BENCHMARK_H}")


# message(STATUS "Catch2 v2 include dirs: ${_catch_inc}")

# find_package(Catch2 2 REQUIRED CONFIG)

# if(Catch2_FOUND)
# message(STATUS "Catch2 found")
# get_target_property(_catch_inc Catch2::Catch2WithMain INTERFACE_INCLUDE_DIRECTORIES)
# message(STATUS "Catch2 include dirs: ${_catch_inc}")
# else()
# message(STATUS "Catch2 not found")
# endif()
set(LIB_NAME "dirtyNet")
set(TEST_NAME ${LIB_NAME}_test)
set(BENCH_NAME ${LIB_NAME}_bench)

add_subdirectory(benchmarks)
add_subdirectory(core)
add_subdirectory(tests)

target_include_directories(${TEST_NAME}
    PRIVATE
    ${catch2_v2_SOURCE_DIR}/single_include
)

add_custom_target(tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
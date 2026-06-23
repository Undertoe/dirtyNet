cmake_minimum_required(VERSION 3.23)

project(dirtyNet VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DIRTYNET_BUILD_PERF "Build dirtyNet performance targets" ON)

include(CTest)

add_subdirectory(lib)
add_subdirectory(exec)
add_subdirectory(sandbox)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

if(DIRTYNET_BUILD_PERF)
    add_subdirectory(perf)
endif()

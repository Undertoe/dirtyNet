project(dirtyNet)

cmake_minimum_required(VERSION 3.22)
include(CTest)

set(CMAKE_CXX_STANDARD 20)
find_package(Catch2)

if(Catch2_FOUND)
    message(STATUS "Catch2 found")
else()
    message(STATUS "Catch2 not found")
endif()

set(LIB_NAME "dirtyNet")
set(TEST_NAME ${LIB_NAME}_test)


add_subdirectory(core)
add_subdirectory(tests)


add_custom_target(tests 
    COMMAND ${CMAKE_MAKE_PROGRAM}
    COMMAND ${CMAKE_MAKE_PROGRAM} -C tests
    COMMAND ${CMAKE_MAKE_PROGRAM} test ARGS=-"-V"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
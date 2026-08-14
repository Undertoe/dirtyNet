find_package(Catch2 3 CONFIG REQUIRED)

file(GLOB_RECURSE DIRTYNET_TEST_SOURCES CONFIGURE_DEPENDS "*.cc")

add_executable(dirtynet_tests ${DIRTYNET_TEST_SOURCES})

target_link_libraries(dirtynet_tests
    PRIVATE
        dirtyNet::dirtynet
        Catch2::Catch2WithMain
)

add_test(NAME dirtynet_tests COMMAND dirtynet_tests)

find_package(Catch2 3 CONFIG REQUIRED)

add_executable(dirtynet_tests
    address/ip.cc
)

target_link_libraries(dirtynet_tests
    PRIVATE
        dirtyNet::dirtynet
        Catch2::Catch2WithMain
)

add_test(NAME dirtynet_tests COMMAND dirtynet_tests)

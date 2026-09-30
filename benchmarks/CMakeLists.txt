find_package(benchmark CONFIG REQUIRED)

add_executable(dirtynet_benchmarks
    address/ip.cc
)

target_link_libraries(dirtynet_benchmarks
    PRIVATE
        dirtyNet::dirtynet
        benchmark::benchmark
)

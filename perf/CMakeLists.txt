find_package(benchmark CONFIG REQUIRED)

add_executable(dirtynet_perf
    dirtynet_perf.cc
)

target_link_libraries(dirtynet_perf
    PRIVATE
        dirtyNet::dirtynet
        benchmark::benchmark
)

file(GLOB SRC "*.cc")

include_directories(../core)

find_package(benchmark REQUIRED)

add_executable(${BENCH_NAME} ${SRC})

target_link_libraries(${BENCH_NAME} ${LIB_NAME} pthread benchmark::benchmark)
target_include_directories(${BENCH_NAME} PRIVATE ${BENCHMARK_INCLUDE_DIRS})
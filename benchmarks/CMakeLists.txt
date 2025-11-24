file(GLOB SRC "*.cc")

include_directories(../core)


add_executable(${BENCH_NAME} ${SRC})

target_link_libraries(${BENCH_NAME} ${LIB_NAME} pthread ${BENCHMARK_LIBRARIES})
target_include_directories(${BENCH_NAME} PRIVATE ${BENCHMARK_INCLUDE_DIRS})
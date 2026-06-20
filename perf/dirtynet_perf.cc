#include <benchmark/benchmark.h>
#include <dirtynet/dirtynet.hh>

static void dirtynet_link_smoke(benchmark::State& state) { for (auto _ : state) benchmark::DoNotOptimize(dirtynet::answer()); }
BENCHMARK(dirtynet_link_smoke);
BENCHMARK_MAIN();

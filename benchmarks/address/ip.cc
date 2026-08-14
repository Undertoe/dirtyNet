#include <benchmark/benchmark.h>

#include <dirtynet/ip.hh>

static void ip_baseline(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::ClobberMemory();
    }
}

BENCHMARK(ip_baseline);
BENCHMARK_MAIN();

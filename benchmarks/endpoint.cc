#include <benchmark/benchmark.h>
#include "endpoint.hh"

static void EndpointParse1(benchmark::State& state) {
    for (auto _ : state) {
        auto ep = dirtyNet::endpoint::parse_endpoint("[::1]:8080");
        benchmark::DoNotOptimize(ep);
    }
}

static void EndpointParse2(benchmark::State& state) {
    for (auto _ : state) {
        auto ep = dirtyNet::endpoint::parse_endpoint("192.168.0.1:8080");
        benchmark::DoNotOptimize(ep);
    }
}

static void EndpointParse3(benchmark::State& state) {
    for (auto _ : state) {
        auto ep = dirtyNet::endpoint("192.168.0.1", "8080");
        benchmark::DoNotOptimize(ep);
    }
}

static void EndpointParse4(benchmark::State& state) {
    for (auto _ : state) {
        auto ep = dirtyNet::endpoint("192.168.0.1", 8080);
        benchmark::DoNotOptimize(ep);
    }
}

BENCHMARK(EndpointParse1);
BENCHMARK(EndpointParse2);
BENCHMARK(EndpointParse3);
BENCHMARK(EndpointParse4);
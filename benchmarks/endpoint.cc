#include "endpoint.hh"

#include <benchmark/benchmark.h>

static void
EndpointParse1(benchmark::State& state)
{
    std::string str = "[5d10:ae41:4c6b:6c39:5d23:f4c1:27ef:65f6]:8080";
    auto ep_test = dirtyNet::endpoint::parse_endpoint(str);
    if (!ep_test.has_value())
    {
        state.SkipWithError("parsing ipv6 failed");
    }
    for (auto _ : state)
    {
        auto ep = dirtyNet::endpoint::parse_endpoint(str);
        benchmark::DoNotOptimize(ep);
    }
}

static void
EndpointParse2(benchmark::State& state)
{
    std::string str = "181.249.232.238:8080";
    auto ep_test = dirtyNet::endpoint::parse_endpoint(str);
    if (!ep_test.has_value())
    {
        state.SkipWithError("parsing ipv4 failed");
    }

    for (auto _ : state)
    {
        auto ep = dirtyNet::endpoint::parse_endpoint(str);

        benchmark::DoNotOptimize(ep);
    }
}

static void
EndpointParse3(benchmark::State& state)
{
    std::string ip = "181.249.232.238";
    std::string ipstr = "8080";

    for (auto _ : state)
    {
        auto ep = dirtyNet::endpoint(ip, ipstr);
        benchmark::DoNotOptimize(ep);
    }
}

static void
EndpointParse4(benchmark::State& state)
{
    std::string ip = "181.249.232.238";
    for (auto _ : state)
    {
        auto ep = dirtyNet::endpoint(ip, 8080);
        benchmark::DoNotOptimize(ep);
    }
}

static void
EndpointParseLocalHost(benchmark::State& state)
{
    std::string str = "localhost:8080";
    for (auto _ : state)
    {
        auto ep = dirtyNet::endpoint::parse_endpoint(str);
        benchmark::DoNotOptimize(ep);
    }
}

static void
InetIPV4Parse(benchmark::State& state)
{
    std::string ip = "181.249.232.238";
    for (auto _ : state)
    {
        in_addr addr{0};
        inet_pton(AF_INET, ip.c_str(), &addr);
        benchmark::DoNotOptimize(addr);
    }
}

static void
InetIPV6Parse(benchmark::State& state)
{
    std::string str = "5d10:ae41:4c6b:6c39:5d23:f4c1:27ef:65f6";
    for (auto _ : state)
    {
        in6_addr addr{0};
        inet_pton(AF_INET6, str.c_str(), &addr);
        benchmark::DoNotOptimize(addr);
    }
}

static void
EndpointPortIPV4(benchmark::State& state)
{
    uint16_t port = 8080;
    std::string ip = "181.249.232.238";
    in_addr addr{0};
    inet_pton(AF_INET, ip.c_str(), &addr);
    for (auto _ : state)
    {
        dirtyNet::endpoint endpoint(addr, port);
        benchmark::DoNotOptimize(endpoint);
        // benchmark::ClobberMemory();
    }
}

static volatile uint32_t sink_addr;
static volatile uint16_t sink_port;

static void
ManualPortIPV4(benchmark::State& state)
{
    uint16_t port = 8080;
    std::string ip = "181.249.232.238";
    in_addr addr{0};
    inet_pton(AF_INET, ip.c_str(), &addr);
    for (auto _ : state)
    {
        uint16_t port = htons(port);
        sink_addr = addr.s_addr;
        sink_port = port;
        benchmark::DoNotOptimize(sink_addr);
        benchmark::DoNotOptimize(sink_port);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(EndpointParse1);
BENCHMARK(EndpointParse2);
BENCHMARK(EndpointParse3);
BENCHMARK(EndpointParse4);
BENCHMARK(EndpointParseLocalHost);
BENCHMARK(InetIPV4Parse);
BENCHMARK(InetIPV6Parse);

BENCHMARK(EndpointPortIPV4);
BENCHMARK(ManualPortIPV4);
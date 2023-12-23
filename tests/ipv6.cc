#include <catch2/catch.hpp>
#include "ipv6.hh"
#include <vector>


TEST_CASE("IPV6 Construction")
{
    {
        dirtyNet::ipv6 ipv6{std::vector{1234u, 123u, 321u, 1234u}};
        REQUIRE(ipv6.Valid());
    }
    {
        // correctly does not compile
        // dirtyNet::ipv6 ipv6{std::vector{123, 321, 123, 321}};
    }
    {
        dirtyNet::ipv6 valid("::1");
        REQUIRE(valid.Valid());
    }
    {
        dirtyNet::ipv6 one("::1");
        dirtyNet::ipv6 two(std::vector{0u, 0u, 0u, 0x1000000u});
        REQUIRE(one.Address() == two.Address());
        for(int i = 0; i < 4; i ++)
        {
            std::cout << std::hex << one.Address().__in6_u.__u6_addr32[i] << std::endl;
        }
    }
}

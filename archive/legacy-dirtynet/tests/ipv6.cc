#include "ipv6.hh"

#include <catch2/catch.hpp>
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
    {  // basic init
        dirtyNet::ipv6 one("::1");
        dirtyNet::ipv6 two(std::vector{0u, 0u, 0u, 0x1000000u});
        REQUIRE(one.Address() == two.Address());
        for (int i = 0; i < 4; i++)
        {
#if defined(__APPLE__)
            std::cout << std::hex << one.Address().__u6_addr.__u6_addr32[i] << std::endl;
// macOS-specific fallback
#elif defined(__linux__)
            std::cout << std::hex << one.Address().__in6_u.__u6_addr32[i] << std::endl;
// linux-specific
#endif
        }
    }
    {  // testing string -> int init
        dirtyNet::ipv6 str("::1");
        REQUIRE(str.String() == "::1");
        dirtyNet::ipv6 num(str.Address());
        REQUIRE(num.Valid());
        REQUIRE(num.String() == str.String());
        REQUIRE(num.String() == "::1");
    }
}

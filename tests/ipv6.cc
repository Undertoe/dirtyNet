#include <catch2/catch.hpp>
#include "ipv6.hh"
#include <vector>


TEST_CASE("IPV6 Construction")
{
    {
        dirtyNet::ipv6 ipv6{std::vector{1234u, 123u, 321u, 1234u}};
        REQUIRE(ipv6.Valid());
    }
}

#include <catch2/catch.hpp>
#include "ip.hh"


TEST_CASE("ip Construction")
{
    {
        dirtyNet::ip ip(dirtyNet::ipv4("192.168.0.1"));
        REQUIRE(ip.Valid());               
    }
    {
        dirtyNet::ip ip(0x01010101u);
    }
}
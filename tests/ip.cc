#include <catch2/catch.hpp>
#include "ip.hh"

#include <variant>


TEST_CASE("ip Construction")
{
    {
        dirtyNet::ip ip(dirtyNet::ipv4("192.168.0.1"));
        REQUIRE(ip.Valid());               
    }
    {
        dirtyNet::ip ip(0x01010101u);
    }
    {
        dirtyNet::ip ip(dirtyNet::ipv4("192.168.9.1"));
        REQUIRE(ip.Type() == dirtyNet::ip_type::ipv4);
    }
    {
        dirtyNet::ip ip("192.168.0.1");
        REQUIRE(ip.Type() == dirtyNet::ip_type::ipv4);
    }
    {
        dirtyNet::ip ip("::1");
        REQUIRE(ip.Type() == dirtyNet::ip_type::ipv6);
        REQUIRE(ip.Valid());
    }
    {
        dirtyNet::ip ip(dirtyNet::ipv4("192.168.0.1"));
        dirtyNet::ipv4 ip4{"192.168.0.1"};
        REQUIRE(ip.Type() == dirtyNet::ip_type::ipv4);
        auto addr = std::get<in_addr>(ip.Address());
        REQUIRE(ip4.Address().s_addr == addr.s_addr);
    }
}
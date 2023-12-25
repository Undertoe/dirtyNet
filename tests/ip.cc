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
        REQUIRE(ip.String() == "::1");
    }
    {
        dirtyNet::ip ip(dirtyNet::ipv4("192.168.0.1"));
        dirtyNet::ipv4 ip4{"192.168.0.1"};
        REQUIRE(ip.Type() == dirtyNet::ip_type::ipv4);
        auto addr = std::get<in_addr>(ip.Address());
        REQUIRE(ip4.Address().s_addr == addr.s_addr);
        REQUIRE(ip.String() == "192.168.0.1");
    }
    {
        dirtyNet::ip ipv4("192.168.0.1");
        REQUIRE(ipv4.Type() == dirtyNet::ip_type::ipv4);
        dirtyNet::ip ipv6("::1");
        REQUIRE(ipv6.Type() == dirtyNet::ip_type::ipv6);
        REQUIRE(ipv4.String() == "192.168.0.1");
        REQUIRE(ipv6.String() == "::1");
        dirtyNet::ip fromIP6(ipv6.Address());
        REQUIRE(fromIP6.String() == "::1");
        dirtyNet::ip fromIP4(ipv4.Address());
        REQUIRE(fromIP4.String() == "192.168.0.1");
        REQUIRE(fromIP4.Valid());
        REQUIRE(fromIP6.Valid());
        REQUIRE(ipv6.Valid());
        REQUIRE(ipv4.Valid());
    }
}
#include <catch2/catch.hpp>
#include "ipv4.hh"


TEST_CASE("IPV4 Construction")
{
    {
        dirtyNet::ipv4 ipv4("192.168.0.1");
        REQUIRE(ipv4.Valid());
        dirtyNet::ipv4 ipv42("192.168.0.2");
        REQUIRE(ipv42.Valid());
        REQUIRE_FALSE(ipv4.Address().s_addr == ipv42.Address().s_addr);
    }
    {
        dirtyNet::ipv4 ipv4("192.168.0.1");
        REQUIRE(ipv4.Valid());
        dirtyNet::ipv4 ipv42("192.168.0.2");
        REQUIRE(ipv42.Valid());
        REQUIRE_FALSE(ipv4.Address() == ipv42.Address());
    }
    {
        dirtyNet::ipv4 str("192.168.0.1");
        dirtyNet::ipv4 num(str.Address().s_addr);
        REQUIRE(num.Valid());
        REQUIRE(num.String() == str.String());
        std::cout << "str.String()[" << str.String() << "]" << std::endl;
        REQUIRE(str.String() == std::string_view("192.168.0.1"));
    }
}
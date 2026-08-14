#include "dirtynet/error/ipv4.hh"
#include <catch2/catch_test_macros.hpp>


#include <dirtynet/ip.hh>
#include <expected>


TEST_CASE("ipv4")
{
    {
        auto test_ip = dirtynet::ipv4::localhost();
        REQUIRE(test_ip.to_string() == "127.0.0.1");
        

        auto test_two = dirtynet::ipv4::from_ip_string("127.0.0.1");
        REQUIRE(test_two == test_ip);
    }
    {
        auto test_bad = dirtynet::ipv4::from_ip_string("1:0:0:1");
        REQUIRE(test_bad == std::unexpected<ipv4_parse_error>{ipv4_parse_error::invalid_address});
    }
}
#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>
#include <dirtynet/error/ip.hh>
#include <catch2/catch_test_macros.hpp>


#include <dirtynet/ip.hh>
#include <expected>



TEST_CASE("ip")
{
    // building from ipv4
    {
        auto localhost_1 = dirtynet::ip::from_ipv4_string("127.0.0.1");
        auto localhost_2 = dirtynet::ip::localhost();
        REQUIRE(localhost_1 == localhost_2);
    }
    {
        auto test_bad = dirtynet::ip::from_ipv4_string("1:0:0:1");
        REQUIRE(test_bad == std::unexpected<dirtynet::ip_parse_error>{dirtynet::ip_parse_error::invalid_address});
        test_bad = dirtynet::ip::from_ipv4_string("257.0.0.1");
        REQUIRE(test_bad == std::unexpected<dirtynet::ip_parse_error>{dirtynet::ip_parse_error::invalid_address});
    }
}
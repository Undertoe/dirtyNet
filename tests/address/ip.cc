#include <catch2/catch_test_macros.hpp>

#include <dirtynet/ip.hh>

TEST_CASE("dirtynet::ip", "ip_ipv4")
{
    {
        auto test_ip = dirtynet::ip::localhost();
        REQUIRE(test_ip.to_string() == "127.0.0.1");
        

        auto test_two = dirtynet::ip::from_ipv4_string("127.0.0.1");
        REQUIRE(test_two == test_ip);
        REQUIRE(test_two->to_string() == "127.0.0.1");
    }
}

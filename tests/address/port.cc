#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <dirtynet/port.hh>

TEST_CASE("port")
{
    dirtynet::port port(9090);
    REQUIRE(port.to_string() == "9090");

    auto native = dirtynet::detail::port_native_access::get(port);
    auto native_port = native.posix();

    REQUIRE(native_port != 9090);
    auto from_native = dirtynet::detail::port_native_access::from_native(native_port);

    std::cout << "testing ports" << std::endl;
    std::cout << "port: " << port.to_string() << std::endl;
    std::cout << "from_native: " << from_native.to_string() << std::endl;

    REQUIRE(port == from_native);
}   


#include "dirtynet/detail/posix/udp_socket.hh"
#include "dirtynet/detail/native_ip.hh"
#include "dirtynet/endpoint.hh"
#include "dirtynet/ip.hh"
#include "dirtynet/port.hh"
#include <catch2/catch_test_macros.hpp>


#include <cstddef>
#include <expected>
#include <netinet/in.h>
#include <sys/socket.h>



// posix tests only
// these verify that we can build generic types from our posix specifics & they look correct.
#if defined(DIRTYNET_PLATFORM_POSIX)


TEST_CASE("dirtynet::detail::posix::udp_socket", "static_ctrs")
{
    {
        using namespace dirtynet::detail::posix;
        auto ogPort = dirtynet::port(1337);
        dirtynet::endpoint ep(dirtynet::ip::localhost(), ogPort);

        auto mSocket = udp_socket::bind(dirtynet::detail::endpoint_native_access::get(ep));

        REQUIRE(mSocket);

        // this block makes sure our bind makes sense
        auto& socket = *mSocket;
        REQUIRE(socket.get_port());
        auto p = *socket.get_port();
        auto port = dirtynet::port::from_native(p);
        REQUIRE(port == ogPort);
    }
}


#endif
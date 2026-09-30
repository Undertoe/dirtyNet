#include "dirtynet/detail/posix/endpoint.hh"
#include "dirtynet/detail/native_endpoint.hh"
#include "dirtynet/detail/native_ip.hh"
#include <catch2/catch_test_macros.hpp>

#include <dirtynet/ip.hh>
#include <expected>
#include <netinet/in.h>
#include <sys/socket.h>


// posix tests only
// these verify that we can build generic types from our posix specifics & they look correct.
#if defined(DIRTYNET_PLATFORM_POSIX)



TEST_CASE("dirtynet::detail::posix::endpoint", "native_ctr")
{
    {
        using namespace dirtynet::detail::posix;
        sockaddr_in server{};
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_port = htons(1337);

        sockaddr* addr = (sockaddr*)& server;

        auto mep = endpoint::from_socket(addr, sizeof(sockaddr_in));

        REQUIRE(mep);
        auto ep = *mep;
        
        auto mipv4 = dirtynet::detail::native::ipv4::from_ip_string("127.0.0.1");
        REQUIRE(mipv4);
        auto address = *mipv4;
        endpoint otherEp{ip{address}, port{1337}};

        auto ip1 = ep.get_ip();
        auto ip2 = otherEp.get_ip();
        REQUIRE(ip1 == ip2);

        auto p1 = ep.get_port();
        auto p2 = otherEp.get_port();
        REQUIRE(p1 == p2);

        REQUIRE(ep == otherEp);

        REQUIRE(ep.to_string() == otherEp.to_string());
        REQUIRE(ep.to_string() == "127.0.0.1:1337");
    }
}

#endif
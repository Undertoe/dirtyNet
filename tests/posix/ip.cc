#include <catch2/catch_test_macros.hpp>

#include <dirtynet/ip.hh>
#include <netinet/in.h>
#include <sys/socket.h>


// posix tests only
// these verify that we can build generic types from our posix specifics & they look correct.
#if defined(DIRTYNET_PLATFORM_POSIX)

TEST_CASE("dirtynet::detail::posix::ip", "posix_ipv4")
{
    {
        std::string addr = "172.168.0.1";
        auto ipv4 = dirtynet::ip::from_ipv4_string(addr);

        in_addr posix_addr{};
        int result = inet_pton(AF_INET, addr.c_str(), &posix_addr);
        REQUIRE(result == 1);

        auto native_ipv4 = dirtynet::detail::ip_native_access::from_native(posix_addr);
        REQUIRE(native_ipv4 == ipv4);
    }
}



#endif

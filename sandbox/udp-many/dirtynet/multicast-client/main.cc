#include "dirtynet/endpoint.hh"
#include "dirtynet/port.hh"
#include "dirtynet/udpsocket.hh"
#include <any>
#include <dirtynet/ip.hh>

#include <charconv>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>
#include <system_error>


namespace
{
    bool parsePort(std::string_view value, std::uint16_t& port)
    {
        unsigned parsed{};
        auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), port);
        if(error != std::errc{} || end != value.data() + value.size())
        {
            return false;
        }

        if(port == 0 || port > std::numeric_limits<std::uint16_t>::max())
        {
            return false;
        }
        return true;
    }


    std::string_view byte_string(std::span<std::byte> bytes)
    {
        return bytes.empty() ? std::string_view{} 
            : std::string_view{
                reinterpret_cast<const char*>(bytes.data()), bytes.size()
            };
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "MULTICAST CLIENT: expected port argument" << std::endl;
        return 1;
    }

    std::uint16_t p{};
    if(!parsePort(argv[1], p))
    {
        std::cout << "MULTICAST CLIENT: invalid port argument " << argv[1] << std::endl;
        return 1;
    }

    dirtynet::endpoint ep{*dirtynet::ip::from_ipv4_string("239.0.0.1"), dirtynet::port{p}};

    auto mMulticast = dirtynet::udp_socket::bind_multicast(ep);

    if(!mMulticast)
    {
        std::cout << "Failed to setup multicast" << std::endl;
        return -1;
    }

    auto mBytes = mMulticast->receive();
    if(!mBytes)
    {
        std::cout << "failed to get bytes on multicast" << std::endl;
        return -1;
    }

    std::cout << "got message [" << byte_string(mBytes->bytes) << "]" << std::endl;
    
    return 0;
}

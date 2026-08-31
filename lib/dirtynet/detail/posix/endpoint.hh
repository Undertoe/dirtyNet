#pragma once
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <type_traits>
#include <utility>
#include <variant>
#include <expected>

#include <netinet/in.h>
#include <unistd.h>


#include <dirtynet/error/endpoint.hh>
#include "ip.hh"
#include "port.hh"


namespace dirtynet::detail::posix {


// POSIX Endpoint conversion to sockaddr*

class endpoint
{
public:

    using socket_addr_t = sockaddr;
    using socket_length_t = socklen_t;
    using socket_addr_v4_t = sockaddr_in;
    using socket_addr_v6_t = sockaddr_in6;

    static constexpr size_t sockaddr_v4_l = sizeof(socket_addr_v4_t);
    static constexpr size_t sockaddr_v6_l = sizeof(socket_addr_v6_t);

    endpoint(const sockaddr_in& addr) : _storage(addr) { }
    endpoint(const sockaddr_in6& addr) : _storage(addr) { }

    static std::expected<endpoint, endpoint_error> from_socket(const socket_addr_t* addr, socklen_t len)
    {
        if(len == sockaddr_v4_l)
        {
            endpoint ep;
            auto& ipv4 = ep._storage.emplace<sockaddr_in>();
            std::memcpy(&ipv4, addr, sockaddr_v4_l);
            return ep;
        }

        return std::unexpected<endpoint_error>(endpoint_error::unsupported_address_type);
        // handle parsing etc, this is used by the posix socket internal api, && will handle the logic to 
        //  return the high level endpoint object later.
    }

    endpoint() = default;

    // member initializer here will set our sockaddr type appropriately
    endpoint(ip ip, port p) 
        : _storage(ip.is_ipv4() ? 
            decltype(_storage)
            {std::in_place_type<sockaddr_in>}
            : decltype(_storage)
            {std::in_place_type<sockaddr_in6>} )
    {
        // pulls the information out of the IP & port
        // and pulls them into the appropriate sockaddr type
        if(ip.is_ipv4())
        {
            ipv4 i = *ip.get_ipv4();
            auto storage = &std::get<sockaddr_in>(_storage);
            storage->sin_addr = i.native();
            storage->sin_port = p.posix();
            storage->sin_family = AF_INET;
        }
        else {
            // currently unimplemented until we put ipv6 together
        }
    }

    // public standardized API
    ip get_ip() const
    {
        if(const auto* ipv4 = std::get_if<sockaddr_in>(&_storage))
        {
            return ip{ipv4->sin_addr};
        }
        else if(const auto* ipv6 = std::get_if<sockaddr_in6>(&_storage))
        {
            return ip{ipv6->sin6_addr};
        }
    }

    port get_port() const 
    {
        if(const auto* ipv4 = std::get_if<sockaddr_in>(&_storage))
        {
            return port::from_native(ipv4->sin_port);
        }
        else if(const auto* ipv6 = std::get_if<sockaddr_in6>(&_storage))
        {
            return port::from_native(ipv6->sin6_port);
        }
    }

    std::string to_string() const 
    {
        return get_ip().to_string() + ":" + get_port().to_string();
    }

    // posix internal API
    std::pair<const sockaddr*, socklen_t> addr_info() const noexcept
    {
        return std::visit(
            [](const auto& active) -> std::pair<const sockaddr*, socklen_t>
            {
                return {reinterpret_cast<const sockaddr*>(&active), static_cast<socklen_t>(sizeof(active))};
            }, _storage
        );
    }

    bool operator==(const endpoint& other) const
    {
        auto [localAddr, localLen] = addr_info();
        auto [otherAddr, otherLen] = other.addr_info();
        if(localLen != otherLen)
        {
            return false;
        }
        return std::memcmp(localAddr, otherAddr, localLen) == 0;
    }

private:

    std::variant<sockaddr_in, sockaddr_in6> _storage;
    
    
};

}
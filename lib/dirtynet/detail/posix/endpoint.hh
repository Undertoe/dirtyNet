#pragma once
#include <string>
#include <sys/socket.h>
#include <type_traits>
#include <variant>
#include <expected>

#include <netinet/in.h>
#include <unistd.h>


#include "ip.hh"


namespace dirtynet::detail::posix {

enum class endpoint_error{
    unsupported_address_type,
    invalid_length,
    malformed_address_data,
};

inline constexpr std::string_view endpoint_error_string(endpoint_error e)
{
    switch(e)
    {
        case dirtynet::detail::posix::endpoint_error::invalid_length:
            return "invalid length";
        case dirtynet::detail::posix::endpoint_error::unsupported_address_type:
            return "unsupported address type";
        case dirtynet::detail::posix::endpoint_error::malformed_address_data:
            return "malformed address data";
    }
}

// POSIX Endpoint conversion to sockaddr*

class endpoint
{
public:
    static std::expected<endpoint, endpoint_error> from_socket(const sockaddr* addr, socklen_t len)
    {
        // handle parsing etc, this is used by the posix socket internal api, && will handle the logic to 
        //  return the high level endpoint object later.
    }

    endpoint(ip ip, port p)
    {
        
    }

    // public standardized API
    std::string to_string() const
    {
        // returns the parsed ip object, will implement this later
        return "";
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

private:

    // constructed through our factory func, which handles parsing of the sockaddr / length expected
    //  from various POSIX socket information.
    endpoint(const sockaddr_in& addr) : _storage(addr) { }
    endpoint(const sockaddr_in6& addr) : _storage(addr) { }

    std::variant<sockaddr_in, sockaddr_in6> _storage;
    
    
};

}
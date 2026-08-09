#pragma once
#include <string>
#include <type_traits>
#include <variant>

#include <netinet/in.h>
#include <unistd.h>


#include "ip.hh"


namespace dirtynet::detail::posix {

// POSIX Endpoint conversion to sockaddr*

class endpoint
{
public:
    // ctr still deciding on

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

    std::variant<sockaddr_in, sockaddr_in6> _storage;
    
    
};

}
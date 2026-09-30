#pragma once

#include <string_view>

namespace dirtynet
{

enum class ip_parse_error
{
    unsupported_address_family, 
    invalid_address,
    unknown_failure,
};

inline constexpr std::string_view ipv4_parse_error_string(ip_parse_error e)
{
    switch(e)
    {
        case ip_parse_error::unsupported_address_family:
            return "unsupported address family";
        case ip_parse_error::invalid_address:
            return "invalid address";
        case ip_parse_error::unknown_failure:
            return "unknown error / native failure";
    }
}
}
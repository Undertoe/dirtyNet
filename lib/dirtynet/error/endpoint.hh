#pragma once

#include <string_view>

namespace dirtynet
{

enum class endpoint_error{
    unsupported_address_type,
    invalid_length,
    malformed_address_data,
};

inline constexpr std::string_view endpoint_error_string(endpoint_error e)
{
    switch(e)
    {
        case endpoint_error::invalid_length:
            return "invalid length";
        case endpoint_error::unsupported_address_type:
            return "unsupported address type";
        case endpoint_error::malformed_address_data:
            return "malformed address data";
    }
}

}
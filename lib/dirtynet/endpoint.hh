#pragma once

#include <optional>
#include <string>

#include <dirtynet/ip.hh>
#include "detail/native_endpoint.hh"

namespace dirtynet {

namespace detail {
struct endpoint_native_access;
}

// Endpoint composition will be introduced here during the IP pass.

class endpoint
{
public:
    // endpoint();

    std::string to_string() const
    {
        if(!_strCache)
        {
            _strCache = _native.to_string();
        }
        return *_strCache;
    }

private:
    detail::native::endpoint _native;
    mutable std::optional<std::string> _strCache;

    friend struct detail::endpoint_native_access;
};


namespace detail
{
struct endpoint_native_access
{
    static const native::endpoint& get(const dirtynet::endpoint& val) noexcept
    {
        return val._native;
    }
};
}

} // namespace dirtynet

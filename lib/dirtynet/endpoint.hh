#pragma once

#include <optional>
#include <string>

#include "ip.hh"
#include "detail/native_endpoint.hh"

namespace dirtynet {

namespace detail {
struct endpoint_native_access;
}

// Endpoint composition will be introduced here during the IP pass.

class endpoint
{
public:
    endpoint(const ip& i, const port& p) : _native(detail::ip_native_access::get(i), p._storage)
    {

    }

    std::string to_string() const
    {
        if(!_strCache)
        {
            _strCache = _native.to_string();
        }
        return *_strCache;
    }

private:

    endpoint(detail::native::endpoint native) : _native(std::move(native)) {}

    detail::native::endpoint _native;
    mutable std::optional<std::string> _strCache;

    friend struct detail::endpoint_native_access;
    friend struct ip;
};


namespace detail
{
struct endpoint_native_access
{
    static const native::endpoint& get(const dirtynet::endpoint& val) noexcept
    {
        return val._native;
    }


    static dirtynet::endpoint from_native(const detail::native::endpoint native)
    {   
        return endpoint(native);
    }
};
}

} // namespace dirtynet

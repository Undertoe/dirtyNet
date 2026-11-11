#pragma once

#include <optional>
#include <string>

#include "dirtynet/port.hh"
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
    endpoint(const ip& i, const port& p) : _native(detail::ip_native_access::get(i), detail::port_native_access::get(p))
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

    bool operator==(const endpoint& other)
    {
        return _native == other._native;
    }

    port get_port() const 
    {
        return port::from_native(_native.get_port());
    }

    std::optional<ip> get_ip() const 
    {
        auto mipv4 = _native.get_ip().get_ipv4();
        if(!mipv4)
        {
            return std::nullopt;
        }
        return detail::ip_native_access::from_native(mipv4->native());
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

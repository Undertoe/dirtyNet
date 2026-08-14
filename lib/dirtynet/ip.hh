#pragma once

#include <expected>
#include <optional>
#include <string>

#include "dirtynet/detail/native_ip.hh"
#include <netinet/in.h>
#include <dirtynet/error/ipv4.hh>
namespace dirtynet {

// ip types that will be used for construction of our lower level ip values.

// forward declare our access helpers 
namespace detail {
struct ipv4_native_access;
struct ip_native_access;
}

class ipv4
{
public:


    static std::expected<ipv4, ipv4_parse_error> from_ip_string(std::string_view sv)
    {
        auto native = detail::native::ipv4::from_ip_string(sv);
        if(native.has_value())
        {
            return ipv4(native.value());
        }
        return std::unexpected<ipv4_parse_error>(native.error());
    }    

    constexpr static ipv4 localhost() 
    {   
        return ipv4(detail::native::ipv4::localhost());
    }

    std::string to_string() const
    {
        if(!_strCache)
        {
            _strCache = _native.to_string();
        }
        return *_strCache;
    }


    bool operator==(const ipv4& other) const
    {
        return _native == other._native;
    }


    std::strong_ordering operator <=> (const ipv4& other)
    {
        return _native <=> other._native;
    }

private:

    ipv4(const detail::native::ipv4 ip) : _native(ip) {}

    detail::native::ipv4 _native;
    mutable std::optional<std::string> _strCache;

    friend struct detail::ipv4_native_access;
};


class ipv6
{
public:

private:
    detail::native::ipv6 _native;

    // friend struct ipv6_native_access;
};


class ip
{
public:
    
    
private:
    // ip(detail::native::ip native) : _native(std::move(native)) {}
    
    detail::native::ip _native;

    friend struct detail::ip_native_access;
};

    
namespace detail {

    struct ipv4_native_access 
    {
        static const native::ipv4& get(const dirtynet::ipv4& val) noexcept
        {
            return val._native;
        }

        static dirtynet::ipv4 from_native(detail::native::ipv4_internal internal)
        {
            return dirtynet::ipv4(native::ipv4::from_native(internal));
        }
    };

    // need to implement the ip_native_access & add friend access to the important types
    struct ip_native_access
    {
        static const native::ip& get(const dirtynet::ip& val) noexcept
        {
            return val._native;
        }

        // static dirtynet::ip from_native(const detail::native::ip native)
        // {
        //     return dirtynet::ip(native::ipv4::from_native(native));
        // }
    };
}
    

} // namespace dirtynet

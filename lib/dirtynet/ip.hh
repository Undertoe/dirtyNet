#pragma once

#include <expected>
#include <optional>
#include <string>

#include "dirtynet/detail/native_ip.hh"
#include "dirtynet/port.hh"
#include <dirtynet/error/ip.hh>
namespace dirtynet {

// forward declare our access helpers 
namespace detail {
struct ip_native_access;
}


class ip
{
public:


    static std::expected<ip, ip_parse_error> from_ipv4_string(std::string_view sv)
    {
        auto m_ipv4 = detail::native::ip::from_ipv4_string(sv);
        if(!m_ipv4)
        {
            return std::unexpected<ip_parse_error>{m_ipv4.error()};
        }
        return ip(*m_ipv4);
    }


    static constexpr ip localhost()
    {
        return ip(detail::native::ip::localhost());
    }

    static constexpr ip any()
    {
        return ip{detail::native::ip::any()};
    }
    

    bool operator==(const ip& other) const
    {
        return other._native == _native;
    }
    
    std::string to_string() const 
    {
        return _native.to_string();
    }

private:
    ip(const detail::native::ip& native) : _native(native) {}
    ip(detail::native::ip&& native) : _native(std::move(native)) {}
    
    detail::native::ip _native;

    friend struct detail::ip_native_access;
};

    
namespace detail {

    // need to implement the ip_native_access & add friend access to the important types
    struct ip_native_access
    {
        static const native::ip& get(const dirtynet::ip& val) noexcept
        {
            return val._native;
        }

        static dirtynet::ip from_native(detail::native::ipv4::internal_t internal)
        {
            return dirtynet::ip(detail::native::ip{internal});
        }



        // static dirtynet::ip from_native(const detail::native::ip native)
        // {
        //     return dirtynet::ip(native::ipv4::from_native(native));
        // }
    };
}
    

} // namespace dirtynet

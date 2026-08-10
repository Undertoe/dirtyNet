#pragma once

#include "dirtynet/detail/native_ip.hh"
#include <netinet/in.h>
namespace dirtynet {

// ip types that will be used for construction of our lower level ip values.

namespace detail {
struct ip_native_access;
struct port_native_access;
}

    class ipv4
    {
    public:

    private:
        detail::native::ipv4 _native;

        // friend struct ipv4_native_access;
    };


    class ipv6
    {
    public:

    private:
        detail::native::ipv6 _native;

        // friend struct ipv6_native_access;
    };

    class port
    {
    public:

    private:
        detail::native::port _native;
        
        friend struct detail::port_native_access;
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
    // need to implement the ip_native_access & add friend access to the important types
    struct ip_native_access
    {
        static const native::ip& get(const dirtynet::ip& val) noexcept
        {
            return val._native;
        }

        // static dirtynet::ip from_native(const detail::native::ip native)
        // {
        //     return ip(native);
        // }
    };
}
    

} // namespace dirtynet

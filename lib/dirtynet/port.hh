#pragma once


#include "detail/native_port.hh"
#include "ip.hh"
#include <cstdint>

#include <compare>
#include <string>


namespace dirtynet {

namespace detail {
struct port_native_access;
}

class port
{
public:
    port(uint16_t p) : _native(p) {}


    std::string to_string() const
    {
        return _native.to_string();
    }

    bool operator==(const port& other) const
    {
        return _native == other._native;
    }


    std::strong_ordering operator <=> (const port& other)
    {
        return _native <=> other._native;
    }

private:

    port(const detail::native::port& p) : _native(p) {}

    detail::native::port _native;
    friend struct detail::port_native_access;
};

namespace detail
{
struct port_native_access
{
    static const native::port& get(const dirtynet::port& val) noexcept
    {
        return val._native;
    }

    static dirtynet::port from_native(uint16_t p)
    {
        return dirtynet::port(native::port::from_native(p));
        // auto po = native::port::from_native(p);
        // return dirtynet::port(po);
    }
};
}

}
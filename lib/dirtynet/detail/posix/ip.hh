#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <expected>

#include <dirtynet/error/ipv4.hh>

namespace dirtynet::detail::posix {

// POSIX IP conversion support will be introduced here during the IP pass.

class ipv4
{
public:

    using internal_t = in_addr;

    static ipv4 from_native(internal_t t)
    {
        return ipv4(t);
    }

    static std::expected<ipv4, ipv4_parse_error> from_ip_string(std::string_view sv)
    {
        std::string str{sv};
        in_addr address{};
        int result = inet_pton(AF_INET, str.c_str(), &address);
        if(result == 1)
        {
            return ipv4{address};
        }
        else if(result == 0)
        {
            return std::unexpected<ipv4_parse_error>{ipv4_parse_error::invalid_address};
        }
        switch(errno)
        {
            case EAFNOSUPPORT:
                return std::unexpected<ipv4_parse_error>{ipv4_parse_error::unsupported_address_family};
            default:
                return std::unexpected<ipv4_parse_error>{ipv4_parse_error::unknown_failure};
        }
        return std::unexpected<ipv4_parse_error>{ipv4_parse_error::unknown_failure};  
    }

    static constexpr ipv4 localhost()
    {
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return ipv4(in_addr{0x0100007Fu});
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return ipv4(in_addr{0x7F000001u});
#else
#warning "could not determine target byte order at compile time so your localhost() is resolving to non-constexpr.  Defaulting to a htonl implementation for localhost()"
        return ipv4(in_addr{htonl(INADDR_LOOPBACK)});
#endif
    }

    std::string to_string() const 
    {
        std::array<char, INET_ADDRSTRLEN> buffer{0};
        auto res = inet_ntop(AF_INET, &_addr, buffer.data(), buffer.size());
        return std::string(res);
    }

    // static std::expected<ipv4, ipv4_parse_error> hostlookup(std::string_view sv)
    // {
    //     return std::unexpected<ipv4_parse_error>{ipv4_parse_error::unknown_failure};
    // }

    // // this needs to be finished but we will handle that later
    // static std::expected<std::vector<ipv4>, ipv4_parse_error> hostlookup_all(std::string_view sv)
    // {
    //     addrinfo info{.ai_family = AF_INET};
    //     addrinfo* results{nullptr};
    //     std::string hostname(sv);
    //     int status = getaddrinfo(hostname.c_str(), nullptr, &info, &results);

    //     if(status != 0)
    //     {
    //         return std::unexpected<ipv4_parse_error>{ipv4_parse_error::unknown_failure};
    //     }


    //     return {};
    // }



    bool operator==(const ipv4& other) const
    {
        return _addr.s_addr == other._addr.s_addr;
    }


    std::strong_ordering operator <=> (const ipv4& other)
    {
        return _addr.s_addr <=> other._addr.s_addr;
    }


private:

    constexpr ipv4(in_addr addr) : _addr(addr) { } 

    in_addr _addr;

};

class ipv6{

};


class ip // not sure if needed?
{
    

};


} // namespace dirtynet::detail::posix

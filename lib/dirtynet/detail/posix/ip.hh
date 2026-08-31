#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


#include <string_view>
#include <array>
#include <variant>
#include <optional>
#include <string>
#include <expected>

#include <dirtynet/error/ip.hh>

namespace dirtynet::detail::posix {

// POSIX IP conversion support will be introduced here during the IP pass.

class ipv4
{
public:

    using internal_t = in_addr;

    constexpr ipv4(internal_t addr) : _addr(addr) { } 

    static std::expected<ipv4, ip_parse_error> from_ip_string(std::string_view sv)
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
            return std::unexpected<ip_parse_error>{ip_parse_error::invalid_address};
        }
        switch(errno)
        {
            case EAFNOSUPPORT:
                return std::unexpected<ip_parse_error>{ip_parse_error::unsupported_address_family};
            default:
                return std::unexpected<ip_parse_error>{ip_parse_error::unknown_failure};
        }
        return std::unexpected<ip_parse_error>{ip_parse_error::unknown_failure};  
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

    internal_t native() const 
    {
        return _addr;
    }


private:
    internal_t _addr;

};

class ipv6{

public:

    using internal_t = in6_addr;

    ipv6() = default;
    ipv6(internal_t internal) {}

    std::string to_string() const 
    {
        return "unimplemented";
    }

    bool operator==(const ipv6& other) const
    {
        return true;
    }

    internal_t native() const 
    {
         return internal_t{};
    }

};


class ip
{
public:
    
    ip(const ipv4& i) : _storage(i) {}
    ip(ipv4&& i) : _storage(i) {} 
    ip(const ipv6& i) : _storage(i) {}
    ip(ipv6&& i) : _storage(i) {} 

    ip(ipv4::internal_t internal) : _storage(internal) { }
    ip(ipv6::internal_t internal) : _storage(internal) { }


    static std::expected<ip, ip_parse_error> from_ipv4_string(std::string_view sv)
    {
        auto m_ipv4 = ipv4::from_ip_string(sv);
        if(!m_ipv4)
        {
            return std::unexpected<ip_parse_error>{m_ipv4.error()};
        }
        return ip{*m_ipv4};
    }

    // to implement later for now.
    // static std::expected<ip, ip_parse_error> from_ipv6_string(std::string_view sv)
    // {

    // }

    static std::expected<ip, ip_parse_error> from_ip_string(std::string_view sv)
    {
        auto m_ipv4 = ipv4::from_ip_string(sv);
        if(m_ipv4)
        {
            return ip(*m_ipv4);
        }
        // do ipv6 stuff

        return std::unexpected<ip_parse_error>{ip_parse_error::invalid_address};
    }

    static constexpr ip localhost()
    {
        return ip(ipv4::localhost());
    }

    std::string to_string() const 
    {
        return std::visit(
            [](const auto& active) {
                return active.to_string();
            }, 
            _storage
        );
        // return "unimplemented";
    }

    bool is_ipv4() const 
    {
        return std::holds_alternative<ipv4>(_storage);
    }

    bool is_ipv6() const 
    {
        return std::holds_alternative<ipv6>(_storage);
    }

    std::optional<ipv4> get_ipv4() const 
    {
        if(std::holds_alternative<ipv4>(_storage))
        {
            return std::get<ipv4>(_storage);
        }
        return std::nullopt;
    }

    std::optional<ipv6> get_ipv6() const 
    {
        if(std::holds_alternative<ipv6>(_storage))
        {
            return std::get<ipv6>(_storage);
        }
        return std::nullopt;
    }

    bool operator==(const ip& other) const
    {
        auto eq = other._storage == _storage;
        return eq;
    }

private:

    std::variant<ipv4, ipv6> _storage;
};


} // namespace dirtynet::detail::posix

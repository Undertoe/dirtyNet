#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ranges>

#include <array>

#include "util.hh"



namespace dirtyNet
{
class ipv6
{
public:
    template<std::ranges::sized_range Range>
    requires std::same_as<std::ranges::range_value_t<Range>, uint32_t>
    ipv6(const Range& range)
    {
        if(range.size() != 4)
        {
            return;
        }

        size_t index{0}; 
        for(const auto & v : range)
        {
            _addr.__in6_u.__u6_addr32[index++] = v;
        }

        _valid = inet_ntop(AF_INET6, &_addr, &_addrStr[0], INET6_ADDRSTRLEN);
    }

    ipv6(const in6_addr& addr);
    ipv6(std::string_view sv);

    bool Valid() const;
    in6_addr Address() const;
    std::string_view String() const;

private:
    in6_addr _addr{0};
    std::array<char, INET6_ADDRSTRLEN> _addrStr{'\0'};
    bool _valid{false};

};
}

bool operator==(const in6_addr& lhs, const in6_addr& rhs);
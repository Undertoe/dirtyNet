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

        _valid = true;
    }

    ipv6(std::string_view sv)
    {
        _valid = inet_pton(AF_INET6, std::string(sv).c_str(), &_addr) == 1;
    }

    bool Valid() const;
    in6_addr Address() const;
    std::string IP() const;

private:
    in6_addr _addr{0};
    bool _valid{false};

};
}
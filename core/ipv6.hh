#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>

#include <array>
#include <ranges>
#include <string>
#include <string_view>

#include "util.hh"

namespace dirtyNet
{
class ipv6
{
public:
    template <std::ranges::sized_range Range>
        requires std::same_as<std::ranges::range_value_t<Range>, uint32_t>
    ipv6(const Range& range)
    {
        if (range.size() != 4)
        {
            return;
        }

        size_t index{0};
        for (const auto& v : range)
        {
#if defined(__APPLE__)
            _addr.__u6_addr.__u6_addr32[index++] = v;
// macOS-specific fallback
#elif defined(__linux__)
            _addr.__in6_u.__u6_addr32[index++] = v;
// linux-specific
#endif
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
    mutable std::array<char, INET6_ADDRSTRLEN> _addrStr{'\0'};
    mutable bool _valid{false};
};
}  // namespace dirtyNet

bool operator==(const in6_addr& lhs, const in6_addr& rhs);
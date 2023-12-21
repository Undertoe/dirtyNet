
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
    ipv6(const Range& range)
    {
        using T = std::ranges::range_value_t<Range>;
        static_assert(std::same_as<T, uint32_t>, "ipv6 requires to use a range of uint32_t");
        if(range.size() != 4)
        {
            return;
        }

        size_t index{0};
        for(auto & v : range)
        {
            _addr.__in6_u.__u6_addr32[index++] = v;
        }

        _valid = true;
    }

    bool Valid() const;

private:
    in6_addr _addr{0};
    bool _valid{false};

};
}
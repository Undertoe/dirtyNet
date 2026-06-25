#include <charconv>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>
#include <system_error>

namespace
{
    bool parsePort(std::string_view value, std::uint16_t& port)
    {
        unsigned parsed{};
        auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), parsed);
        if(error != std::errc{} || end != value.data() + value.size())
        {
            return false;
        }

        if(parsed == 0 || parsed > std::numeric_limits<std::uint16_t>::max())
        {
            return false;
        }

        port = static_cast<std::uint16_t>(parsed);
        return true;
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "UNICAST CLIENT: expected port argument" << std::endl;
        return 1;
    }

    std::uint16_t port{};
    if(!parsePort(argv[1], port))
    {
        std::cout << "UNICAST CLIENT: invalid port argument " << argv[1] << std::endl;
        return 1;
    }

    std::cout << "UNICAST CLIENT: udp-many scaffold starting on port " << port << std::endl;

    return 0;
}

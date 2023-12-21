
#include <string_view>
#include <vector>
#include <optional>
#include <concepts>
#include <charconv>
#include <iostream>
#include <cstring>

namespace dirtyNet
{

std::vector<std::string_view> string_split(std::string_view str, char delimiter);

template<std::integral IntType>
std::optional<IntType> to_int(std::string_view str)
{
    IntType integer{};
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), integer);
    std::cout << ptr << std::endl;
    if(ec == std::errc{} && strlen(ptr) == 0)
    {
        return integer;
    }
    return std::nullopt;
}


}  // namespace Chapter2
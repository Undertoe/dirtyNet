#pragma once

#include <string_view>
#include <vector>
#include <optional>
#include <concepts>
#include <charconv>
#include <iostream>
#include <cstring>
#include <ranges>

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

template<typename Range>
size_t find_end_char(const Range& r, char endChar)
{
    auto it = std::ranges::find(r, endChar);
    if(it != std::ranges::end(r))
    {
        return static_cast<size_t>(std::ranges::distance(std::ranges::begin(r),it));
    }
    return std::ranges::size(r);
}

}  // namespace Chapter2
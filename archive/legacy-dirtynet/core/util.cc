#include "util.hh"



std::vector<std::string_view>
dirtyNet::string_split(std::string_view str, char delim)
{
    std::vector<std::string_view> strs;

    size_t start{0};
    auto next{str.find_first_of(delim)};
    while(next <= std::string_view::npos)
    {
        if(next == std::string_view::npos)
        {
            break;
        }
        strs.emplace_back(str.substr(start, next - start));
        start = next + 1;
        next = str.find_first_of(delim, start);
    }

    strs.emplace_back(str.substr(start, next-start));
    return strs;
}

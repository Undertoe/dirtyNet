#include <catch2/catch.hpp>

#include <iostream>
#include "util.hh"



TEST_CASE("string split")
{
    {
        std::string str{"The Quick BRown Fox Jumped Over The Lazy Dog"};
        auto split = dirtyNet::string_split(str, ' ');
        REQUIRE(split.size() == 9);
    }
}


TEST_CASE("to_int")
{
    {
        std::string pass{"123"};
        auto mNum = dirtyNet::to_int<uint32_t>(pass);
        REQUIRE(mNum.has_value());
        REQUIRE(mNum.value() == 123u);
    }
    {
        std::string fail{"123f"};
        auto mNum = dirtyNet::to_int<uint32_t>(fail);
        REQUIRE_FALSE(mNum.has_value());
    }
}

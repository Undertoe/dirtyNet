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
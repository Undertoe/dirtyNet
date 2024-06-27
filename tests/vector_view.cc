#include <catch2/catch.hpp>
#include "vector_view.hh"

#include <iostream>


TEST_CASE("Vec Construction")
{
    std::vector<int> data;
    for(int i = 0; i < 1000; i ++)
    {
        data.push_back(i);
    }


    VectorView view1(data, 1, 10);
    int starting = 1;
    for(const auto & v : view1)
    {
        REQUIRE(v == starting++);
    }
    REQUIRE(starting == 11);

    REQUIRE(view1.set(1111, 0));
    REQUIRE(data[1] == 1111);

    REQUIRE(view1.at(5) == 6);
    view1.at(5) = 7;
    REQUIRE(data[6] == 7);

    REQUIRE_THROWS(view1.at(10) == 1);

    for(auto & v : view1)
    {
        v = 0;
    }

    for(int i = 1; i < 11; i ++)
    {
        REQUIRE(data[i] == 0);
    }

    // REQUIRE(1 == 0);
}
#include <catch2/catch_test_macros.hpp>
#include <dirtynet/dirtynet.hh>

TEST_CASE("dirtynet library links") { REQUIRE(dirtynet::answer() == 42); }

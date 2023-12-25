#include <catch2/catch.hpp>

#include <stacktrace>
#include <iostream>
#include <expected>

enum class testExpected
{
    one, two
};

std::expected<std::string, testExpected> getTest(bool t)
{
    if(t)
    {
        return "test";
    }
    return std::unexpected{testExpected::one};
}

TEST_CASE("test")
{
    // auto tmp = std::stacktrace::current();
    #ifdef _GLIBCXX_HAVE_STACKTRACE
    std::cout << "stacktrace enabled" << std::endl;
    #else
    std::cout << "no stacktrace enabled" << std::endl;
    #endif

    #if __cplusplus > 202002L
    std::cout << "Proper C++" << std::endl;
    #else
    std::cout << "also improper C++" << std::endl;
    #endif

    auto t = getTest(true);
    auto f = getTest(false);

    REQUIRE(t == "test");
    REQUIRE(f.error() == testExpected::one);
}
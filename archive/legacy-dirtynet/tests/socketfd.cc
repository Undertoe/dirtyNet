#include <catch2/catch.hpp>
#include "socketfd.hh"

TEST_CASE("SocketFD Move Semantics")
{
    using dirtyNet::detail::SocketFD;

    // Create a SocketFD with a dummy file descriptor
    SocketFD sock1(42);
    REQUIRE(sock1.get() == 42);

    // Move construct sock2 from sock1
    SocketFD sock2(std::move(sock1));
    REQUIRE(sock2.get() == 42);
    REQUIRE(sock1.get() == -1); // sock1 should be invalid after move

    // Move assign sock3 from sock2
    SocketFD sock3;
    sock3 = std::move(sock2);
    REQUIRE(sock3.get() == 42);
    REQUIRE(sock2.get() == -1); // sock2 should be invalid after move

    // Reset sock3 and ensure it closes the fd
    sock3.reset();
    REQUIRE(sock3.get() == -1);
}
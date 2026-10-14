#include "dirtynet/endpoint.hh"
#include "dirtynet/ip.hh"
#include "dirtynet/port.hh"
#include <catch2/catch_test_macros.hpp>
#include <span>
#include <string_view>
#include <utility>


#include "dirtynet/udpsocket.hh"


TEST_CASE("dirtynet::udp_socket::static_ctrs", "static_ctrs")
{
        using namespace dirtynet;
    {
        endpoint ep{ip::localhost(), port{1337}};
        auto msock = udp_socket::open();
        REQUIRE(msock);
        auto& sock = *msock;
        REQUIRE(*sock.get_port() == port{0});
        REQUIRE(msock->is_open());


        auto msockb = udp_socket::bind(ep);
        REQUIRE(msockb);
        auto& sockb = *msockb;
        REQUIRE(sockb.get_port());
        REQUIRE(*sockb.get_port() == port{1337});
        REQUIRE(msockb->is_open());
    }
    {
        endpoint ep{ip::localhost(), port{1337}};
        auto msock = udp_socket::bind(ep);
        auto& sock = *msock;
        auto sockB = std::move(sock);
        REQUIRE(!sock.is_open());
        REQUIRE(*sockB.get_port() == port{1337});
    }
    {
        endpoint ep{ip::localhost(), port{1337}};
        auto msock = udp_socket::bind(ep);
        auto& sock = *msock;
        auto msockB = udp_socket::open();
        auto& sockB = *msockB;
        sockB = std::move(sock);
        REQUIRE(!sock.is_open());
        REQUIRE(*sockB.get_port() == port{1337});
    }
}

TEST_CASE("dirtynet::udp_socket::send-recv", "send-recv")
{
    using namespace dirtynet;
    {
        endpoint ep{ip::localhost(), port{1337}};
        auto sendSock = udp_socket::open();
        auto recvSock = udp_socket::bind(ep);
        REQUIRE(sendSock);
        REQUIRE(recvSock);

        std::string data{"12345"};
        auto sent = sendSock->send_to(ep, std::as_bytes(std::span(data.data(), data.size())));
        REQUIRE(sent);
        REQUIRE(*sent == data.size());

        auto got = recvSock->receive();
        REQUIRE(got);
        REQUIRE(got->bytes.size() == 5);
        std::string_view got_sv = got->bytes.empty() ? std::string_view{} : std::string_view{
            reinterpret_cast<const char*>(got->bytes.data()), got->bytes.size()
        };
        REQUIRE(got_sv == data);
    }
    {
        // check move semantics.
        endpoint ep{ip::localhost(), port{1338}};
        auto sendSock = udp_socket::open();
        auto tmp = udp_socket::bind(ep);
        auto recvSock = std::move(*tmp);

        std::string data{"54321"};
        auto sent = sendSock->send_to(ep, std::as_bytes(std::span(data.data(), data.size())));
        REQUIRE(sent);
        REQUIRE(*sent == data.size());

        auto got = recvSock.receive();
        REQUIRE(got);
        REQUIRE(got->bytes.size() == 5);
        std::string_view got_sv = got->bytes.empty() ? std::string_view{} : std::string_view{
            reinterpret_cast<const char*>(got->bytes.data()), got->bytes.size()
        };
        REQUIRE(got_sv == data);
    }

}

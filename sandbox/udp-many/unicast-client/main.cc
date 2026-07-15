#include <charconv>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>
#include <system_error>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
namespace
{
    bool parsePort(std::string_view value, std::uint16_t& port)
    {
        unsigned parsed{};
        auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), parsed);
        if(error != std::errc{} || end != value.data() + value.size())
        {
            return false;
        }

        if(parsed == 0 || parsed > std::numeric_limits<std::uint16_t>::max())
        {
            return false;
        }

        port = htons(static_cast<std::uint16_t>(parsed));
        return true;
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "UNICAST CLIENT: expected port argument" << std::endl;
        return 1;
    }

    std::uint16_t port{};
    if(!parsePort(argv[1], port))
    {
        std::cout << "UNICAST CLIENT: invalid port argument " << argv[1] << std::endl;
        return 1;
    }

    std::cout << "UNICAST CLIENT: udp-many scaffold starting on port " << ntohs(port) << std::endl;
    sockaddr_in server{0};
    sockaddr_in client{0};
    auto sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        std::cout << "UNICAST CLIENT: client failed to create socket " << sockfd << std::endl;
        return sockfd;
    }

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = port;

    if(auto bound = bind(sockfd, reinterpret_cast<sockaddr*>(&local), sizeof(local)); bound < 0)
    {
        std::cout << "UNICAST CLIENT: Client failed to bind to localaddr. error: " << bound << std::endl;
        return bound;
    }

    // server.sin_family = AF_INET;
    // server.sin_addr.s_addr = inet_addr("127.0.0.1");
    // server.sin_port = port;
    socklen_t len = sizeof(server);

    char buffer[1024];
    int n = recvfrom(sockfd, buffer, 1024, MSG_WAITALL, (sockaddr*)&server, &len);
    buffer[n] = '\0';
    std::cout << "UNICAST CLIENT: Recieved message from server: " << std::string(buffer) << std::endl;


    close(sockfd);

    std::cout << "UNICAST CLIENT: Exited" << std::endl;

    return 0;
}

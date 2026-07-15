#include <asm-generic/socket.h>
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
        std::cout << "MULTICAST CLIENT: expected port argument" << std::endl;
        return 1;
    }

    std::uint16_t port{};
    if(!parsePort(argv[1], port))
    {
        std::cout << "MULTICAST CLIENT: invalid port argument " << argv[1] << std::endl;
        return 1;
    }

    std::cout << "MULTICAST CLIENT: starting on port " << ntohs(port) << std::endl;

    // setup the socket
    sockaddr_in server{0};
    sockaddr_in client{0};
    auto sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        std::cout << "MULTICAST CLIENT: client failed to create socket " << sockfd << std::endl;
        return sockfd;
    }

    // set the socket to "reuse",  letting multiple sockets use the same port
    int reuse = 1;
    if(auto sockopt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); sockopt < 0)
    {
        std::cout << "MULTICAST CLIENT: set sock opt for multicast REUSEADDR failed with error: " << sockopt << std::endl;
        return sockopt;
    }



    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY); // multicast bind is to any
    server.sin_port = port;  // still need mutlicast port
    socklen_t len = sizeof(server);

    // bind so we can listen for incoming across the group
    auto bindfd = bind(sockfd, (sockaddr*)&server, len); 
    
    if(bindfd < 0)
    {
        std::cout << "MULTICAST CLIENT: bind failed with error: " << bindfd << std::endl;
        return bindfd;
    }



    // setup the group for multicast
    // here we need the actual address we're listening to
    ip_mreq multicast_mreq;
    multicast_mreq.imr_multiaddr.s_addr = inet_addr("239.0.0.1");   // must be between 224.0.0.0 -> 239.255.255.255
    multicast_mreq.imr_interface.s_addr = htonl(INADDR_ANY);    // this can be anything, we bind to any for now.
    if(auto multicast_sockopt = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_mreq, sizeof(multicast_mreq)); multicast_sockopt < 0)
    {
        std::cout << "MULTICAST CLIENT: setting socket option for multicast ADD MEMBERSHIP failed: " << multicast_sockopt << std::endl;
        return multicast_sockopt;
    }


    char buffer[1024];
    int n = recvfrom(sockfd, buffer, 1024, MSG_WAITALL, (sockaddr*)&server, &len);
    buffer[n] = '\0';
    std::cout << "MULTICAST CLIENT: Recieved message from server: " << std::string(buffer) << std::endl;


    close(sockfd);

    std::cout << "MULTICAST CLIENT: Exited" << std::endl;

    return 0;
}

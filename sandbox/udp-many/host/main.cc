#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <stdint.h>


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>



std::vector<uint16_t> unicast_ports = {9000, 9001, 9002, 9003};
uint16_t multicast_port = 10000;


int main()
{
    std::cout << "HOST: udp-many scaffold starting" << std::endl;


    // sleep for 100 ms to let the clients start up & connect
    std::this_thread::sleep_for(std::chrono::milliseconds{100});


    std::cout << "HOST: Setting up to send to unicast-clients" << std::endl;

    std::vector<std::pair<int, std::shared_ptr<sockaddr_in>>> client_targets;
    for(const auto & port : unicast_ports)
    {
        auto sock = socket(AF_INET, SOCK_DGRAM, 0);
        if(sock < 0)
        {
            std::cout << "HOST: socket creation failed" << std::endl;
        }
        auto addr = std::make_shared<sockaddr_in>(0);
        addr->sin_family = AF_INET;
        addr->sin_port = port;
        addr->sin_addr.s_addr = INADDR_ANY;

        client_targets.push_back({sock, addr});
    }

    std::cout << "HOST: setting up to send to multicast client" << std::endl;
    auto multi_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned char ttl{1};
    unsigned char loop{1};

    if(auto sockop = setsockopt(multi_sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)); sockop < 0)
    {
        std::cout << "HOST: Failed to set multicast socket ttl to 1. Error: " << sockop << std::endl;
        return sockop;
    }
    if(auto sockop = setsockopt(multi_sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)); sockop < 0)
    {
        std::cout << "HOST: Failed to set multicast socket loop to 1. Error: " << sockop << std::endl;
        return sockop;
    }
    in_addr interface{0};
    interface.s_addr = htonl(INADDR_ANY);
    if(auto sockop = setsockopt(multi_sockfd, IPPROTO_IP, IP_MULTICAST_IF, &interface, sizeof(interface)); sockop < 0)
    {
        std::cout << "HOST: Failed to set multicast socket interface. Error: " << sockop << std::endl;
        return sockop;
    }

    sockaddr_in multicast_send{0};
    multicast_send.sin_family = AF_INET;
    multicast_send.sin_port = multicast_port;
    inet_pton(AF_INET, "239.0.0.1", &multicast_send.sin_addr);
    // we should now be ready to send.

    
    
    return 0;
}

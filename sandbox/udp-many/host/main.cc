#include <chrono>
#include <cstdint>
#include <flat_map>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <stdint.h>


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>



std::vector<uint16_t> unicast_ports = {htons(9000), htons(9001), htons(9002), htons(9003)};
uint16_t multicast_port = htons(10000);
std::vector<std::pair<int, std::shared_ptr<sockaddr_in>>> client_targets;
int multi_sockfd{0};

int cleanup(int retval)
{
    for(auto & [fd, target] : client_targets)
    {
        std::cout << "closing fd" << std::endl;
        close(fd);
    }
    std::cout << "Closing multisock fd" << std::endl;
    close(multi_sockfd);

    std::cout << "Cleaned up" << std::endl;
    return retval;
}

int main()
{
    std::cout << "HOST: udp-many scaffold starting" << std::endl;


    // sleep for 100 ms to let the clients start up & connect
    std::this_thread::sleep_for(std::chrono::milliseconds{100});


    std::cout << "HOST: Setting up to send to unicast-clients" << std::endl;

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
    multi_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned char ttl{1};
    unsigned char loop{1};

    if(auto sockop = setsockopt(multi_sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)); sockop < 0)
    {
        std::cout << "HOST: Failed to set multicast socket ttl to 1. Error: " << sockop << std::endl;
        return cleanup(sockop);
    }
    if(auto sockop = setsockopt(multi_sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)); sockop < 0)
    {
        std::cout << "HOST: Failed to set multicast socket loop to 1. Error: " << sockop << std::endl;
        return cleanup(sockop);
    }
    in_addr interface{0};
    interface.s_addr = htonl(INADDR_ANY);
    if(auto sockop = setsockopt(multi_sockfd, IPPROTO_IP, IP_MULTICAST_IF, &interface, sizeof(interface)); sockop < 0)
    {
        std::cout << "HOST: Failed to set multicast socket interface. Error: " << sockop << std::endl;
        return cleanup(sockop);
    }

    sockaddr_in multicast_send{0};
    multicast_send.sin_family = AF_INET;
    multicast_send.sin_port = multicast_port;
    inet_pton(AF_INET, "239.0.0.1", &multicast_send.sin_addr);
    // we should now be ready to send.
    std::cout << "Host sleeping to start" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "HOST: writing greetings to unicast clients" << std::endl; 
    // start buy looping over our unicast ports to send our greeting.
    for(auto & [fd, target] : client_targets)
    {
        auto portString = std::to_string(ntohs(target->sin_port));
        std::cout << "Writing to port [" << portString << "]" << std::endl;
        std::string greeting = "hello to port " + portString; 

        auto bytesWritten = sendto(fd, greeting.data(), greeting.length(), 0, reinterpret_cast<sockaddr*>(target.get()), sizeof(*target));
        if(bytesWritten < 1)
        {
            std::cout << "HOST: Failed to write out greeting [" << greeting << "]" << std::endl;
            return cleanup(bytesWritten); 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{250});
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{125});
    std::cout << "HOST: Writing Greetings to multicast clients" << std::endl;
    std::string greeting = "Hello multicast!";
    auto bytesWritten = sendto(multi_sockfd, greeting.data(), greeting.size(), 0, reinterpret_cast<sockaddr*>(&multicast_send), sizeof(multicast_send));

    if(bytesWritten < 1)
    {
        std::cout << "HOST: Failed to write out greeting [" << greeting << "]" << std::endl;
        return cleanup(bytesWritten); 
    }

       
    return cleanup(0);
}

#include "dirtynet/endpoint.hh"
#include "dirtynet/error/udp.hh"
#include "dirtynet/port.hh"
#include <cerrno>
#include <chrono>
#include <dirtynet/ip.hh>
#include <dirtynet/udpsocket.hh>

#include <iostream>
#include <span>
#include <thread>
#include <memory>

std::vector<dirtynet::port> unicast_ports
{
    dirtynet::port{9000}, 
    dirtynet::port{9001},
    dirtynet::port{9002},
    dirtynet::port{9003},
};
dirtynet::port multicast_port{10000};

std::vector<std::pair<dirtynet::endpoint, dirtynet::udp_socket>> client_targets;

int main() 
{
    std::cout << "HOST: udp-many scaffold starting" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds{100});

    std::cout << "HOST: Setting up to send to unicast-clients" << std::endl;

    for(const auto & port : unicast_ports)
    {
        dirtynet::endpoint ep{dirtynet::ip::any(), port};
        auto mSocket = dirtynet::udp_socket::open();
        if(!mSocket)
        {
            auto err = errno;

            std::cout << "Socket bind failed for socket using port " << port.to_string() << " Erno " << err << std::endl;
            return -1;
        }
        client_targets.emplace_back(ep, std::move(*mSocket));
    }
    
    std::cout << "HOST: setting up to send to multicast client" << std::endl;
    auto mMultiSock = dirtynet::udp_socket::bind_multicast(dirtynet::endpoint{*dirtynet::ip::from_ipv4_string("239.0.0.1"), multicast_port});

    if(!mMultiSock)
    {
        auto err = errno;
        std::cout << "Multicast bind failed, errno: " << err << std::endl;
        return -1;
    }

    std::cout << "Host waiting for clients to warm up" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::string unicastGreeting{"Hello from server "}; 
    std::string multicastGreeting{"Hello multicast"};

    for(auto & [ep, sock] : client_targets)
    {
        std::string msg = unicastGreeting + ep.get_port().to_string();
        auto bytesWritten = sock.send_to(ep, std::as_bytes(std::span(msg.data(), msg.length())));
        if(!bytesWritten)
        {
            std::cout << "Failed to write bytes out to port " << ep.get_port().to_string() << std::endl;
            return -1;
        }
        std::cout << "Wrote out bytes [" << *bytesWritten << "] to port [" << ep.get_port().to_string() << "]" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    auto bytesWritten = mMultiSock->send_to(dirtynet::endpoint{*dirtynet::ip::from_ipv4_string("239.0.0.1"), multicast_port}, std::as_bytes(std::span(multicastGreeting.data(), multicastGreeting.size())));

    if(!bytesWritten)
    {
        std::cout << "Failed to write out to multicast" << std::endl;
        return -1;
    }
    std::cout << "wrote [" << *bytesWritten << "] to multicast" << std::endl;

    return 0;
}

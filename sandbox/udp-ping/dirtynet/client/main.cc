#include "dirtynet/endpoint.hh"
#include "dirtynet/port.hh"
#include "dirtynet/udpsocket.hh"
#include <cstddef>
#include <dirtynet/ip.hh>

#include <span>
#include <iostream>
#include <vector>

int main() 
{
    std::cout << "CLIENT: Starting up client" << std::endl;

    std::string greeting{"Hello from client"};

    using namespace dirtynet;
    endpoint ep{ip::localhost(), port{8090}};

    auto mServer = dirtynet::udp_socket::open();
    if(!mServer)
    {
        return -1;
    }

    auto& server = *mServer;
    
    auto sent = server.send_to(ep, std::as_bytes(std::span{greeting.data(), greeting.size()}));

    if(!sent)
    {
        std::cout << "Failed to send to server" << std::endl;
        return -1;
    }
    std::cout << "CLIENT: sent " << *sent << " bites to server" << std::endl;

    auto mPacket = server.receive();
    if(!mPacket)
    {
        std::cout << "Failure to get packet back from server" << std::endl;
        return -1;
    }

    auto& [bytes, sender, truncated] = *mPacket;
    std::string_view msgBack = bytes.empty() 
        ? std::string_view{} 
        : std::string_view{
            reinterpret_cast<const char*>(bytes.data()), bytes.size()
        };
    std::cout << "CLIENT: Recieved message from server: " << msgBack << std::endl;

    return 0;
}

#include "dirtynet/endpoint.hh"
#include "dirtynet/port.hh"
#include "dirtynet/udpsocket.hh"
#include <dirtynet/ip.hh>

#include <ratio>
#include <span>
#include <iostream>
#include <thread>
#include <vector>

int main() 
{
    std::cout << "SERVER: Starting up server" << std::endl;

    static constexpr int portInt{8090};

    using namespace dirtynet;
    auto mServer = udp_socket::bind(endpoint{ip::any(), port(portInt)});
    if(!mServer)
    {
        std::cout << "Server failed to bind, exiting" << std::endl;
        return -1;
    }
    auto& server = *mServer;
    
    std::string serverResponse{"hello from server"};

    auto rec = server.receive();

    std::this_thread::sleep_for(std::chrono::milliseconds{5});
    if(!rec)
    {
        std::cout << "Server failed to recieve data" << std::endl;
        return -1;
    }

    auto& [bytes, sender, truncated] = *rec;
    std::string_view clientMsg = bytes.empty() 
        ? std::string_view{} 
        : std::string_view{
            reinterpret_cast<const char*>(bytes.data()), bytes.size()
        };
    std::cout << "SERVER got message from client: " << clientMsg << std::endl;
    
    auto sent = server.send_to(sender, std::as_bytes(std::span{serverResponse.data(), serverResponse.size()}));

    if(!sent)
    {
        std::cout << "server failed to send bytes" << std::endl;
        return -1;
    }

    std::cout << "SERVER: message sent" << std::endl;
    return 0;
}

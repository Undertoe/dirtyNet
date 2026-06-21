#include <netdb.h> 
#include <netinet/in.h> 
#include <ostream>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <iostream>
#include <chrono>
#include <iostream>

#include "shared.hh"

int main()
{
    std::cout << "TCP client hello world" << std::endl;

    static constexpr int port{8090};
    sockaddr_in serverAddr{0};
    sockaddr_in clientAddr{0};

    auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        std::cout << "CLIENT: socket creation failed: " << sockfd << std::endl;
        return sockfd;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);

    if(auto conn = connect(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)); conn != 0)
    {
        std::cout << "CLIENT: connect failed: " << conn << std::endl;
        return conn;
    }

    char buffer[1024]{0};
    tcp_packet::packet packet = tcp_packet::create_ping();
    auto encodedPacket = packet.encode();

    auto bitesWritten = write(sockfd, &encodedPacket[0], encodedPacket.size());
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;
    auto bitesRead = read(sockfd, buffer, sizeof(buffer));
    // buffer[bitesRead] = '\0';
    auto serverMsg = tcp_packet::packet(buffer, bitesRead);
    
    std::cout << "CLIENT: recieved packet type from server: " << serverMsg.type_string() << std::endl;

    if(serverMsg.hdr._type != tcp_packet::type::pong)
    {
        std::cout << "INVALID PACKET BACK FOR TEST, EXITING" << std::endl;
        return -1;
    }

    packet = tcp_packet::create_msg("Hello from client!");
    encodedPacket = packet.encode();
    bitesWritten = write(sockfd, &encodedPacket[0], encodedPacket.size());
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;

    
    bzero(buffer, sizeof(buffer));
    bitesRead = read(sockfd, buffer, sizeof(buffer));
    serverMsg = tcp_packet::packet(buffer, bitesRead);
    if(serverMsg.hdr._type != tcp_packet::type::greeting)
    {
        std::cout << "CLIENT: EXPECTED GREETING, EXITING" << std::endl;
        return -1;
    }
    std::cout << "CLIENT: Recieved message from server: " << serverMsg.msg << std::endl;


    std::cout << "Client writing quit to server" << std::endl;
    packet = tcp_packet::create_msg("QUIT");
    encodedPacket = packet.encode();
    bitesWritten = write(sockfd, &encodedPacket[0], encodedPacket.size());
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;


    std::cout << "CLIENT: Exiting" << std::endl;
    close(sockfd);

    return 0;
}

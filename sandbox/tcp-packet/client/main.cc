#include <netdb.h> 
#include <netinet/in.h> 
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
    auto bitesRead = read(sockfd, buffer, sizeof(buffer) - 1);
    // buffer[bitesRead] = '\0';
    
    std::cout << "CIENT: got message from server [" << serverMsg << "]" << std::endl;

    

    bitesWritten = write(sockfd, &msg2[0], msg2.size());
    std::cout << "CLIENT: Wrote message [" << msg2 << "]" << std::endl; 
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;

    bzero(buffer, sizeof(buffer));
    bitesRead = read(sockfd, buffer, sizeof(buffer) - 1);
    buffer[bitesRead] = '\0';
    serverMsg = std::string(buffer);
    std::cout << "CIENT: got message from server [" << serverMsg << "]" << std::endl;

    std::cout << "CLIENT: Exiting" << std::endl;
    close(sockfd);

    return 0;
}

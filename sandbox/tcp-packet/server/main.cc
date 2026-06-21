#include "shared.hh"


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

int main()
{
    std::cout << "SERVER: Starting\n";

    static constexpr int port{8090};

    sockaddr_in server{0};
    sockaddr_in client{0};


    // create & bind the socket
    auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "SERVER: Socket failed to create " << sockfd << std::endl;
        return sockfd;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if(auto bound = bind(sockfd, (sockaddr*) &server, sizeof(server)); bound < 0)
    {
        std::cout << "SERVER: binding to socket " << sockfd << " failed. " << bound << std::endl;
        return bound;
    }



    // set socket to listen & accept new connections

    if(auto list = listen(sockfd, 5); list != 0)
    {
        std::cout << "SERVER: Soocket listen failed: " << list << std::endl;
        return list;
    }

    std::cout << "SERVER: Now listening" << std::endl;
    socklen_t len = sizeof(client);

    auto connfd = accept(sockfd, (sockaddr*) & client, &len); 
    if (connfd < 0)
    {
        std::cout << "SERVER: accept() failed: " << connfd << std::endl;
        return connfd;
    }

    // run for a while: read from the socket
    // print in the server what we got
    // modify the text a lil & send it back
    char buff[1024];
    bool running { true };
    while(running)
    {
        // while(true);
        bzero(buff, sizeof(buff));
        auto charRead = read(connfd, buff, sizeof(buff)-1);
        std::cout << "SERVER: chars read: " << charRead << std::endl; 
        if(charRead == 0)
        {
            std::cout << "SERVER: Recieved 0 bytes from client, assumed dead connection" << std::endl;
            return -1;
        }

        auto clientMsg = tcp_packet::packet(buff, charRead);

        tcp_packet::type msgType = clientMsg.hdr._type;
        switch(msgType)
        {
            case tcp_packet::type::ping:
            {
                auto response = tcp_packet::create_pong();
                std::cout << "SERVER: Responding to ping with pong" << std::endl;
                auto bytesout = response.encode();
                auto bytesWritten = write(connfd, &bytesout[0], bytesout.size());
                std::cout << "SERVER: wrote " << bytesWritten << " bytes" << std::endl;
                break;
            }
            case tcp_packet::type::greeting:
            {
                std::cout << "SERVER: Greeting receieved [" << clientMsg.msg << "]\n";
                auto response = tcp_packet::create_msg("Hello from server [" + clientMsg.msg + "]");
                auto bytesOut = response.encode();
                auto bytesWritten = write(connfd, &bytesOut[0], bytesOut.size());
                std::cout << "SERVER: wrote " << bytesWritten << " bytes" << std::endl;
                if(clientMsg.msg == "QUIT")
                {
                    running = false;
                }
            }
        }
    }
    std::cout << "SERVER: Exiting" << std::endl;

    
    // close socket

    close(sockfd);
    
}

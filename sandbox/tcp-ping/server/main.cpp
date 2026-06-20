
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#include <strings.h>

#include <string>
#include <iostream>
#include <chrono>

// https://www.geeksforgeeks.org/c/tcp-server-client-implementation-in-c/
int main()
{
    std::cout << "SERVER: Starting";

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
        bzero(buff, sizeof(buff));
        auto charRead = read(connfd, buff, sizeof(buff)-1);
        buff[charRead] = '\0';
        std::string clientMsg{std::string(buff)};
        std::cout << "SERVER: Recieved message : " << clientMsg << std::endl;

        std::string response{"Hello from server[" + clientMsg + "]"};

        auto writtenBites = write(connfd, &response[0], response.size());
        std::cout << "SERVER: Bytes sent back: " << writtenBites << std::endl;
        if(clientMsg == "QUIT")
        {
            running = false;
        }
    }
    std::cout << "SERVER: Exiting" << std::endl;

    
    // close socket

    close(sockfd);
    
    return 0;
}

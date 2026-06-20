
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()

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
    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cout << "SERVER: Socket failed to create " << sock << std::endl;
        return sock;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if(auto bound = bind(sock, (sockaddr*) &server, sizeof(server)); bound < 0)
    {
        std::cout << "SERVER: binding to socket " << sock << " failed. " << bound << std::endl;
        return bound;
    }



    // set socket to listen & accept new connections

    // run for a while: read from the socket
    // print in the server what we got
    // modify the text a lil & send it back
    
    // close socket
    
    return 0;
}

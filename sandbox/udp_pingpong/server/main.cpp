
#include <netinet/in.h>
#include <ratio>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <chrono>
#include <thread>


int main()
{
    std::cout << "SERVER: Starting up server" << std::endl;

    static constexpr int port{8090};

    sockaddr_in server{0};
    sockaddr_in client{0};

    auto sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        std::cout << "Socket creation failed: " << sock << std::endl;
        return sock;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    
    if( auto b = bind(sock, (sockaddr*)&server, sizeof(server)); b < 0)
    {
        std::cout << "Bind failed for server: " << b << std::endl;
        return b;
    }

    socklen_t len {sizeof(client)};

    char buffer[1024];
    std::string response{"hello from server"};
    
    auto n = recvfrom(sock, (char*)buffer, 1024, MSG_WAITALL, (sockaddr*) &client, &len);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    buffer[n] = '\0';
    std::cout << "SERVER: recieved mesage from client " <<  std::string(buffer) << std::endl;

    sendto(sock, (char*) &response[0], response.length(), MSG_CONFIRM, (sockaddr*) &client, len);

    std::cout << "SERVER: message sent" << std::endl;
    
    close(sock);
    return 0;
}

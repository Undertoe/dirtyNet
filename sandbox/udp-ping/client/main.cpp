

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>



int main()
{
    std::cout << "CLIENT: Starting up client" << std::endl;

    char buffer[1024];
    std::string greeting{"Hello from client"};
    static constexpr int port{8090};

    sockaddr_in server{0};
    sockaddr_in client{0};

    auto sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        std::cout << "CLIENT: client failed to create socket " << sockfd << std::endl;
        return sockfd;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(port);

    socklen_t len = sizeof(server);

    auto bitesSent = sendto(sockfd, &greeting[0], greeting.size(), MSG_CONFIRM, (sockaddr*)&server, sizeof(server));
    std::cout << "CLIENT: sent " << bitesSent << " bites to server" << std::endl;

    int n = recvfrom(sockfd, buffer, 1024, MSG_WAITALL, (sockaddr*)&server, &len);

    buffer[n] = '\0';
    std::cout << "CLIENT: Recieved message from server: " << std::string(buffer) << std::endl;

    close(sockfd);

    return 0;
}

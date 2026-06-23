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
#include <set>

#include "shared.hh"

enum class ReadReturnStatus{
    ok_packets, 
    closed,
    socket_error,
    protocol_error,
};

// we assume the buffer is going to be overwritten, this is simply to avoid multiple allocations of the buffer.
std::pair<ReadReturnStatus, std::vector<tcp_packet::packet>> get_packets(int sockfd, std::span<char> buffer, std::vector<char>& unprocessedBytes)
{
    std::vector<tcp_packet::packet> retval;

    // loops to read everything available until we're out of data.
    auto bytesRead = read(sockfd, buffer.data(), buffer.size()); 

    if(bytesRead == 0)
    {
        return {ReadReturnStatus::closed, {}};
    }
    if(bytesRead < 0){

        return {ReadReturnStatus::socket_error, {}};
    }

    unprocessedBytes.insert(unprocessedBytes.end(), buffer.data(), buffer.data() + bytesRead);

    while(unprocessedBytes.size() > 0)
    {
        auto result = tcp_packet::parse_packet(unprocessedBytes);

        // if we hit this, we have unprocessed data that doesn't create a complete packet, so we wait until we see more data.
        if(result.status == tcp_packet::parse_status::incomplete)
        {
            break;
        }

        // an error happened, we should actually report an error in a real situation here
        if(result.status == tcp_packet::parse_status::invalid)
        {
            return {ReadReturnStatus::protocol_error, {}};
        }

        // this should be a real packet, but lets just nullcheck the optional to be sure.
        if(!result.pkt)
        {
            break;
        }
        // now we have a real packet
        retval.push_back(*result.pkt);
        unprocessedBytes.erase(unprocessedBytes.begin(), unprocessedBytes.begin() + result.bytes_consumed);
    }

    return {ReadReturnStatus::ok_packets,retval};
}

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

    // set up our buffer
    char inBuffer[128]{0};
    constexpr size_t inbufferlen = sizeof(inBuffer);
    std::vector<char> incomingBuffer;

    // WRITE: PING PACKET
    tcp_packet::packet packet = tcp_packet::create_ping();
    auto encodeData = packet.encode();
    if(encodeData.size() == 0)
    {
        std::cout << "CLIENT: Failed to encode packet ping" << std::endl;
    }

    auto bitesWritten = write(sockfd, encodeData.data(), encodeData.size());
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;

    auto result = get_packets(sockfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
    auto& [errorReturn, packets] = result;
    auto allPackets = packets;

    // we are expecting a single pong response.
    if(errorReturn != ReadReturnStatus::ok_packets)
    {
        std::cout << "CLIENT: Invalid packet count returned, expected 3 packets, got " << packets.size() << std::endl;
        return -1;
    }    
    while(allPackets.size() < 1)
    {
        result = get_packets(sockfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
        if (errorReturn != ReadReturnStatus::ok_packets)
        {
            std::cout << "CLIENT: got an error while connecting " << std::endl;
            return -1;
        }
        allPackets.insert(allPackets.end(), packets.begin(), packets.end());
    }
    if (errorReturn != ReadReturnStatus::ok_packets)
    {
        std::cout << "CLIENT: Got incorrect return code on read" << std::endl;
        return -1;
    }
    for(const auto & packet : allPackets)
    {
        std::cout << "CLIENT: got packet type " << packet.type_string() << std::endl;
        if(packet.hdr != tcp_packet::type::pong)
        {
            std::cout << "CLIENT: Did not get a pong when expected, exiting" << std::endl;
            return -1;
        }
    }
    packet = tcp_packet::create_msg("Hello from client!");
    encodeData = packet.encode();
    bitesWritten = write(sockfd, encodeData.data(), encodeData.size());
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;


    // Now expecting multiple small packets
    // packets will be sent one at a time from the other connection
    // if we dont get all of our packets continue to read
    result = get_packets(sockfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
    if(errorReturn != ReadReturnStatus::ok_packets)
    {
        std::cout << "CLIENT: Invalid packet count returned, expected 3 packets, got " << packets.size() << std::endl;
        return -1;
    }    
    allPackets = packets;
    while(allPackets.size() < 3)
    {
        result = get_packets(sockfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
        if (errorReturn != ReadReturnStatus::ok_packets)
        {
            std::cout << "CLIENT: got an error while connecting " << std::endl;
            return -1;
        }
        allPackets.insert(allPackets.end(), packets.begin(), packets.end());
    }

    if(allPackets[0].hdr != tcp_packet::type::pong)
    {
        std::cout << "CLIENT: Got invalid packet for packet 0: " << allPackets[0].type_string() << std::endl;
    }
    if(allPackets[1].hdr != tcp_packet::type::ping)
    {
        std::cout << "CLIENT: Got invalid packet for packet 0: " << allPackets[1].type_string() << std::endl;
    }
    if(allPackets[2].hdr != tcp_packet::type::greeting)
    {
        std::cout << "CLIENT: Got invalid packet for packet 0: " << allPackets[2].type_string() << std::endl;
    }
    std::cout << "CLIENT: Greeting from server: " << allPackets[2].msg << std::endl;
    

    // next we write a message thats going to be larger than what we write
    // 33*16 = 528 bytes, we will write it all but only 256 are read on the other side at a time.
    std::string builtMsg(33*16, '\0');
    for(int i = 0; i < 33; i ++)
    {
        char nextChar = static_cast<char>('0' + i);
        for(int j = 0; j < 16; j ++)
        {
            builtMsg[i*16 + j] = nextChar;
        }
    }
    packet = tcp_packet::create_msg(builtMsg);
    encodeData = packet.encode();
    bitesWritten = write(sockfd, encodeData.data(), encodeData.size());
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;


    // we are now expecting 3 large messages of variable length
    // messages are all encoded together and sent at once.
    // message 1 is 100 bytes greeting (100 read)
    // message 2 is 50 bytes  greeting (150 read)
    // message 3 is 150 bytes greeting (300 read)
    // this should test the general reading case.

    result = get_packets(sockfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);

    if(errorReturn != ReadReturnStatus::ok_packets)
    {
        std::cout << "CLIENT: Invalid packet return when trying to read 3packet" << std::endl;
        return -1;
    }    
    allPackets = packets;
    
    // for this read we will continue reading while we dont have all 3 packets unprocessed bites
    while( allPackets.size() < 3 )
    { 
        result = get_packets(sockfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
        if (errorReturn != ReadReturnStatus::ok_packets)
        {
            std::cout << "CLIENT: got an error while reading " << std::endl;
            return -1;
        }
        allPackets.insert(allPackets.end(), packets.begin(), packets.end());
    }
    std::cout << "CLIENT: Outputting greeting" << std::endl;
    std::cout << "\t"   << allPackets[0].msg 
              << "\n\t" << allPackets[1].msg 
              << "\n\t" << allPackets[2].msg 
              << std::endl;
    


    std::cout << "Client writing quit to server" << std::endl;
    packet = tcp_packet::create_msg("QUIT");
    auto encodedPacket = packet.encode();
    bitesWritten = write(sockfd, encodedPacket.data(), encodedPacket.size());
    std::cout << "CLIENT: Wrote " << bitesWritten << " bytes" << std::endl;


    std::cout << "CLIENT: Exiting" << std::endl;
    close(sockfd);

    return 0;
}

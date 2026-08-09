#include "shared.hh"


#include <cstddef>
#include <cstdio>
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
    std::cout << "SERVER: read " << bytesRead << " bytes" << std::endl;

    unprocessedBytes.insert(unprocessedBytes.end(), buffer.data(), buffer.data() + bytesRead);

    while(unprocessedBytes.size() > 0)
    {
        std::cout << "SERVER: parsing" << std::endl;
        auto result = tcp_packet::parse_packet(unprocessedBytes);

        // if we hit this, we have unprocessed data that doesn't create a complete packet, so we wait until we see more data.
        if(result.status == tcp_packet::parse_status::incomplete)
        {
            std::cout << "Incomplete" << std::endl;
            break;
        }

        // an error happened, we should actually report an error in a real situation here
        if(result.status == tcp_packet::parse_status::invalid)
        {
            std::cout << "Invalid" << std::endl;
            return {ReadReturnStatus::protocol_error, {}};
        }

        // this should be a real packet, but lets just nullcheck the optional to be sure.
        if(!result.pkt)
        {
            std::cout << "edge case none-type packet" << std::endl;
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

    int yes=1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
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
    std::cout << "SERVER: Got connection" << std::endl;

    // setup our buffers
    char inBuffer[256];
    constexpr size_t inbufferlen = sizeof(inBuffer);
    std::vector<char> incomingBuffer;

    std::cout << "Waiting for packet" << std::endl;
    auto result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
    auto& [errorReturn, packets] = result;
    auto allPackets = packets;

    // we are expecting a single pong response.
    if(errorReturn != ReadReturnStatus::ok_packets)
    {
        std::cout << "SERVER: Invalid packet count returned, expected 3 packets, got " << packets.size() << std::endl;
        return -1;
    }    
    while(allPackets.size() < 1)
    {
        result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
        if (errorReturn != ReadReturnStatus::ok_packets)
        {
            std::cout << "SERVER: got an error while connecting " << std::endl;
            return -1;
        }
        allPackets.insert(allPackets.end(), packets.begin(), packets.end());
    }
    if(allPackets.size() > 1)
    {
        std::cout << "SERVER: Got too many packets from client " << allPackets.size() << std::endl;
        for(const auto & p : allPackets)
        {
            std::cout << p.type_string() << std::endl;
        }
        return -1;
    }
    if(allPackets[0].hdr != tcp_packet::type::ping)
    {
        std::cout << "SERVER: Did not get ping from client" << std::endl;
        return -1;
    }
    std::cout << "SERVER: got ping from client, responding" << std::endl;

    // write out pong
    std::cout << "SERVER: responding with PONG" << std::endl;
    tcp_packet::packet packet = tcp_packet::create_pong();
    auto encodedData = packet.encode();
    if(encodedData.size() == 0)
    {
        std::cout << "SERVER: Failed to encode pong data" << std::endl;
    }
    auto bytesWritten = write(connfd, encodedData.data(), encodedData.size());
    std::cout << "SERVER Wrote " << bytesWritten << " bytes" << std::endl;


    // read in: 
    // greeting from Client
    std::cout << "SERVER: Expecting greeting" << std::endl;
    result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
    allPackets = packets;
    while(allPackets.size() < 1)
    {
        std::cout << "Still getting data" << std::endl;
        result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
        if (errorReturn != ReadReturnStatus::ok_packets)
        {
            std::cout << "SERVER: got an error while connecting " << std::endl;
            return -1;
        }
        allPackets.insert(allPackets.end(), packets.begin(), packets.end());
    }
    if(allPackets.size() > 1)
    {
        std::cout << "SERVER: Got too many packets from client " << allPackets.size() << std::endl;
        return -1;
    }
    if(allPackets[0].hdr != tcp_packet::type::greeting)
    {
        std::cout << "SERVER: Did not get ping from client" << std::endl;
        return -1;
    }
    std::cout << "SERVER got greeting from Client: " << allPackets[0].msg << std::endl;


    // write out multiple packets: 
    // pong, ping, "hello world" 
    // messages are sent one at a time
    std::cout << "SERVER: Sending 3 packets" << std::endl;
    std::cout << "SERVER: Sending Pong" << std::endl;
    packet = tcp_packet::create_pong();
    encodedData = packet.encode();
    bytesWritten = write(connfd, encodedData.data(), encodedData.size());
    std::cout << "SERVER: Bytes written: " << bytesWritten << std::endl;
    if(bytesWritten == 0)
    {
        std::cout << "SERVER: Error writing out pong" << std::endl;
        return -1;
    }
    std::cout << "SERVER: Sending Ping" << std::endl;
    packet = tcp_packet::create_ping();
    encodedData = packet.encode();
    bytesWritten = write(connfd, encodedData.data(), encodedData.size());
    if(bytesWritten == 0)
    {
        std::cout << "SERVER: Error writing out ping" << std::endl;
        return -1;
    }
    
    std::cout << "SERVER: [Hello World!]" << std::endl;
    packet = tcp_packet::create_msg("Hello World!");
    encodedData = packet.encode();
    bytesWritten = write(connfd, encodedData.data(), encodedData.size());
    if(bytesWritten == 0)
    {
        std::cout << "SERVER: Error writing out greeting" << std::endl;
        return -1;
    }

    
    // now we read a large single packet that will be greater than 2x our buffer length
    result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
    allPackets = packets;
    while(allPackets.size() < 1)
    {
        result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
        if (errorReturn != ReadReturnStatus::ok_packets)
        {
            std::cout << "SERVER: got an error while connecting " << std::endl;
            return -1;
        }
        allPackets.insert(allPackets.end(), packets.begin(), packets.end());
    }
    if(allPackets.size() > 1)
    {
        std::cout << "SERVER: Got too many packets from client" << std::endl;
        return -1;
    }
    if(allPackets[0].hdr != tcp_packet::type::greeting)
    {
        std::cout << "SERVER: Did not get ping from client" << std::endl;
        return -1;
    }
    std::cout << "SERVER got greeting from client:\n[" << allPackets[0].msg << "]" << std::endl;


    // now we write out our last message
    // 100 character greeting
    // 50  character greeting
    // 150 character greeting
    // these are encoded together and sent at once.
    std::vector<char> fullPacketEncoded;
    {
        std::cout << "making 100 long msg" << std::endl;
        std::string msg;
        msg.resize(100);
        for(int i = 0; i < 50; i ++)
        {
            msg[i] = static_cast<char>('0' + i);
            msg[50+i] = static_cast<char>('0' + i);
        }
        auto pak = tcp_packet::create_msg(msg);
        auto bytes = pak.encode();
        fullPacketEncoded.insert(fullPacketEncoded.end(), bytes.begin(), bytes.end());
    }
    {
        std::cout << "making 50 long msg" << std::endl;
        std::string msg;
        msg.resize(50);
        for(int i = 0; i < 50; i ++)
        {
            msg[0] = static_cast<char>('0' + i);
        }
        auto pak = tcp_packet::create_msg(msg);
        auto bytes = pak.encode();
        fullPacketEncoded.insert(fullPacketEncoded.end(), bytes.begin(), bytes.end());
    }
    {
        std::cout << "making 150 long msg" << std::endl;
        std::string msg;
        msg.resize(150);
        for(int i = 0; i < 50; i ++)
        {
            msg[0] = static_cast<char>('0' + i);
            msg[50+i] = static_cast<char>('0' + i);
            msg[100+i] = static_cast<char>('0' + i);
        }
        auto pak = tcp_packet::create_msg(msg);
        auto bytes = pak.encode();
        fullPacketEncoded.insert(fullPacketEncoded.end(), bytes.begin(), bytes.end());
    }
    bytesWritten = write(connfd, fullPacketEncoded.data(), fullPacketEncoded.size());
    std::cout << "Wrote " << bytesWritten << " bytes" << std::endl;
    if(bytesWritten == 0)
    {
        std::cout << "SERVER: Error writing out pong" << std::endl;
        return -1;
    }


    // finally we wait for quit message to be sent
    result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
    allPackets = packets;
    while(allPackets.size() < 1)
    {
        result = get_packets(connfd, std::span<char>(inBuffer, inbufferlen), incomingBuffer);
        if (errorReturn != ReadReturnStatus::ok_packets)
        {
            std::cout << "SERVER: got an error while connecting " << std::endl;
            return -1;
        }
        allPackets.insert(allPackets.end(), packets.begin(), packets.end());
    }
    if(allPackets.size() > 1)
    {
        std::cout << "SERVER: Got too many packets from client" << std::endl;
        return -1;
    }
    if(allPackets[0].hdr != tcp_packet::type::greeting)
    {
        std::cout << "SERVER: Did not get ping from client" << std::endl;
        return -1;
    }
    if(allPackets[0].msg != "QUIT")
    {
        std::cout << "SERVER: Did not recieve quit from client [" << allPackets[0].msg << "]" << std::endl;
        return -1; 
    }

    close(connfd);
    close(sockfd);
    std::cout << "SERVER: Exiting" << std::endl;

    
    
}

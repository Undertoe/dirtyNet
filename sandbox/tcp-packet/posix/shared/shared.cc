#include "shared.hh"
#include <cstddef>
#include <cstdint>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <vector>
#include <iostream>

namespace tcp_packet
{
    std::string type_as_string(type t)
    {
        switch (t)
        {
            case type::greeting:
                return "greeting";
            case type::invalid:
                return "invalid";
            case type::ping:
                return "ping";
            case type::pong:
                return "pong";
        }
        return "unknown type";
    }


    packet::packet(tcp_packet::type t)
    {
        if(t == type::greeting)
        {
            validPacket = false;
        }
        hdr = t;
    }


    packet::packet(const std::string& str) : msg(str) 
    {
        datalen = str.size();
        hdr = type::greeting;
    }

    std::vector<char> packet::encode() const
    {
        constexpr size_t hdrLength = sizeof(hdr);
        constexpr size_t datalenLen = sizeof(datalen);
        if(!validPacket)
        {
            return {};
        }
        std::vector<char> outBuffer(hdrLength, 0);
        auto networkHdr = htonl(static_cast<uint32_t>(hdr));
        std::memcpy(outBuffer.data(), &networkHdr, hdrLength);
        if(hdr == type::greeting)
        {
            outBuffer.resize(hdrLength + datalenLen + datalen);
            auto networkDatalen = htonl(datalen);
            std::memcpy(outBuffer.data() + hdrLength, &networkDatalen, datalenLen);
            std::memcpy(outBuffer.data() + hdrLength + datalenLen, msg.data(), msg.length());
        }
        return outBuffer;
    }


    // we need to change all parsing to use std::span
    parse_result parse_packet(std::span<char> buffer)
    {
        constexpr size_t headerSize = sizeof(type);
        constexpr size_t lengthSize = sizeof(packet::datalen);
        if(buffer.size() < headerSize)
        {
            std::cout << "miscompare with legth + header: " 
                      << "\n\theader " << headerSize
                      << "\n\tBuffer " << buffer.size() << std::endl;  
            return {parse_status::incomplete, std::nullopt, 0};
        }

        // grab the type 
        uint32_t type_raw{0};
        std::memcpy(&type_raw, buffer.data(), headerSize);
        auto pkt_type = static_cast<tcp_packet::type>(ntohl(type_raw));

        // check for the packet type, we only skip if its a msg we skip it to handle this after the switch statement
        switch(pkt_type)
        {
            case tcp_packet::type::ping:
                return {parse_status::complete, packet{type::ping}, headerSize};
            case tcp_packet::type::pong:
                return {parse_status::complete, packet{type::pong}, headerSize};
            case tcp_packet::type::greeting:
                break;
            default: // default means we didn't get a valid packet type so bail out.
                return {parse_status::invalid, std::nullopt, 0};
        }
        // at this point we're checking to see if we have enough data / a valid msg
        if(buffer.size() < headerSize + lengthSize)
        {
            std::cout << "miscompare with legth + header: " 
                      << "\n\theader " << headerSize
                      << "\n\tlength " << lengthSize
                      << "\n\tBuffer " << buffer.size() << std::endl;  
            return {parse_status::incomplete, std::nullopt, 0};
        }
        
        // check the length, if not enough bytes in the bfufer we return incomplete
        uint32_t raw_len{};
        std::memcpy(&raw_len, buffer.data() + headerSize, sizeof(raw_len));

        uint32_t payloadLen = ntohl(raw_len);
        uint32_t totalLen = headerSize + lengthSize + payloadLen;

        // if we have enough bytes we create the message and return the packet 
        if(buffer.size() < totalLen)
        {            
            std::cout << "miscompare with legth + header: " 
                      << "\n\ttotalLen " << totalLen
                      << "\n\tBuffer " << buffer.size() << std::endl;  
            return {parse_status::incomplete, std::nullopt, 0};
        }
        std::string msg(buffer.data() + headerSize + lengthSize, payloadLen);

        return { parse_status::complete, packet{msg}, totalLen};

    }


    std::string tcp_packet::packet::type_string() const
    {
        return tcp_packet::type_as_string(hdr);
    }

    packet create_ping()
    {
        return packet(type::ping);
    }

    packet create_pong()
    {
        return packet(type::pong);
    }

    packet create_msg(const std::string& msg)
    {
        return packet(msg);
    }
}

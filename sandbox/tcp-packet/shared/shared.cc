#include "shared.hh"
#include <cstddef>
#include <cstdint>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <optional>
#include <string>

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

    bool header::encode(char* ptr, size_t remaining_length) const
    {
        // bounds check
        if(remaining_length < sizeof(_type))
        {
            return false;
        }
        auto type_network = htonl(static_cast<uint32_t>(_type));
        std::memcpy(&ptr[0], &type_network, sizeof(type_network));
        return true;
    }


    packet::packet(tcp_packet::type t)
    {
        if(t == type::greeting)
        {
            validPacket = false;
        }
        hdr._type = t;
    }


    packet::packet(const std::string& str) : msg(str) 
    {
        datalen = msg.size();
        hdr._type = type::greeting;
    }

    bool packet::encode(char* buffer, size_t remainingLength) const
    {
        // checks if we do not have enough space remaining
        if(remainingLength < sizeof(hdr) + datalen)
        {
            return false;
        }

        hdr.encode(buffer, remainingLength);
        buffer += sizeof(hdr);
        if(hdr._type == type::greeting)
        {
            auto tmpLength = ntohl(datalen);
            std::memcpy(buffer, &tmpLength, sizeof(tmpLength));
            buffer += sizeof(tmpLength);
            std::memcpy(buffer, &msg[0], msg.length());
        }
        return true;
    }

    // we need to change all parsing to use std::span
    parse_result tcp_packet::parse_packet(std::span<char> buffer)
    {
        constexpr size_t headerSize = sizeof(header::_type);
        constexpr size_t lengthSize = sizeof(packet::datalen);
        if(buffer.size() < headerSize)
        {
            return {parse_status::invalid, std::nullopt, 0};
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
            return {parse_status::incomplete, std::nullopt, 0};
        }
        std::string msg(buffer.data() + headerSize + lengthSize, payloadLen);

        return { parse_status::complete, packet{msg}, totalLen};

    }


    std::string tcp_packet::packet::type_string() const
    {
        return tcp_packet::type_as_string(hdr._type);
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

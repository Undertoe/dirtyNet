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

    header::header(const char* buffer, size_t count)
    {
        if(count < sizeof(header))
        {
            return;
        }
        uint32_t tmp_type;
        std::memcpy(&tmp_type, &buffer[0], sizeof(_type));
        _type = static_cast<type>(ntohl(tmp_type));
    }

    bool header::encode(char* ptr, size_t remaining_length) const
    {
        // bounds check
        if(remaining_length < sizeof(header))
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

    packet::packet(char* buffer, size_t bufferlen) : hdr(buffer, bufferlen)
    {
        buffer += sizeof(hdr._type);
        memcpy(&datalen, buffer, sizeof(datalen));
        datalen = ntohl(datalen);
        buffer += sizeof(datalen);

        if(bufferlen < datalen - sizeof(datalen) - sizeof(hdr._type))
        {
            validPacket = false;
        }
        msg = std::string(buffer, datalen);
        buffer += datalen;
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

    std::optional<packet> tcp_packet::parse_packet(char* buffer, size_t bufferlen)
    {
        tcp_packet::header header(buffer, bufferlen);
        switch (header._type)
        {
            case tcp_packet::type::greeting:
            {
                packet p(buffer, bufferlen);
                if(p.validPacket)
                {
                    return p;
                }
                return {};
            }
            case tcp_packet::type::ping:
            {
                buffer += sizeof(header._type);
                return packet(tcp_packet::type::ping);
            }
            case tcp_packet::type::pong:
            {
                buffer += sizeof(header._type);
                return packet(tcp_packet::type::pong);
            }
            default:
                return {};
        }
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

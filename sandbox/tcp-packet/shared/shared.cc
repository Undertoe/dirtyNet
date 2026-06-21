#include "shared.hh"
#include <cstdint>
#include <arpa/inet.h>

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
        std::memcpy(&_datalen, &buffer[0], sizeof(_datalen));
        std::memcpy(&tmp_type, &buffer[sizeof(_datalen)], sizeof(_type));
        _datalen = static_cast<uint32_t>(ntohl(_datalen));
        _type = static_cast<type>(ntohl(tmp_type));
    }

    std::vector<uint8_t> header::encode() const 
    {
        if(_type == type::invalid)
        {
            return {};
        }
        std::vector<uint8_t> retval(sizeof(header), 0);
        auto type_network = htonl(static_cast<uint32_t>(_type));
        auto length = htonl(_datalen);
        std::memcpy(&retval[0], &length, sizeof(length));
        std::memcpy(&retval[sizeof(length)], &type_network, sizeof(type_network));
        return retval;
    }

    packet::packet(tcp_packet::type t)
    {
        if(t == type::greeting)
        {
            validPacket = false;
        }
        hdr._type = t;
    }

    packet::packet(const std::string& str) :msg(str) 
    {
        hdr._datalen = msg.size();
        hdr._type = type::greeting;
    }

    packet::packet(const char* buffer, size_t length) : hdr(buffer, length)
    {
        if(hdr._type == type::invalid)
        {
            validPacket = false;
            return;
        }

        if(hdr._datalen > 0)
        {
            msg = std::string(&buffer[(sizeof(hdr))], hdr._datalen);
        }
    }

    std::vector<uint8_t> packet::encode() const
    {   
        if(!validPacket)
        {
            return std::vector<uint8_t>{};
        }
        std::vector<uint8_t> retval(msg.length() + sizeof(hdr), 0);
        std::memcpy(&retval[0], hdr.encode().data(), sizeof(hdr));
        if(hdr._type == type::greeting)
        {
            std::memcpy(&retval[sizeof(hdr)], &msg[0], msg.length());
        }

        return retval;
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

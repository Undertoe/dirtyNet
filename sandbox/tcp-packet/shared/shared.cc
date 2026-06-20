#include "shared.hh"
#include <cstdint>

namespace tcp_packet
{

    packet::packet(type t)
    {
        if(t == type::greeting)
        {
            validPacket = false;
        }
    }

    packet::packet(const std::string& str) :msg(str) 
    {
        header._datalen = msg.size();
        header._type = type::greeting;
    }

    packet::packet(const std::vector<uint8_t>& data) : header(data) {
        // copies the rest of the data as string chars
        
        if(header._type == type::greeting)
        {
            msg = std::string(reinterpret_cast<const char*>(data.data() + sizeof(header)), header._datalen);
        }
    }

    std::vector<uint8_t> packet::encode() const
    {   
        std::vector<uint8_t> retval(msg.length() + sizeof(header), 0);
        std::memcpy(&retval[0], header.encode().data(), sizeof(header));
        std::memcpy(&retval[sizeof(header)], &msg[0], msg.length());
        return retval;
    }

}

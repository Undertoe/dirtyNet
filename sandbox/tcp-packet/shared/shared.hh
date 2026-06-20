#pragma once

#include <codecvt>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <stdint.h>

namespace tcp_packet
{
std::string hello_world();

enum class type : uint32_t{
    ping, pong, greeting,
};


struct header{
    uint32_t _datalen;
    type _type;

    header() = default;
    header(const std::vector<uint8_t> data);

    std::vector<uint8_t> encode() const;
};

struct packet{
    header header; // NOT INCLUDING THE HEADER LENGTH
    std::string msg;
    bool validPacket{true};

    packet() = delete;
    packet(type t);
    packet(const std::string& msg);
    packet(const std::vector<uint8_t> & data);

    std::vector<uint8_t> encode() const;
};

packet create_ping();
packet create_pong();
packet create_msg(const std::string& msg);

}

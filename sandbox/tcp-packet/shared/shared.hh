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
    ping, pong, greeting, invalid,
};

std::string type_as_string(type);


struct header{
    uint32_t _datalen{0};
    type _type{type::invalid};

    header() = default;
    header(const char*, size_t);

    std::vector<uint8_t> encode() const;
};

struct packet{
    header hdr; // NOT INCLUDING THE HEADER LENGTH
    std::string msg;
    bool validPacket{true};

    packet() = delete;
    packet(type t);
    packet(const std::string& msg);
    packet(const char*, size_t);

    std::string type_string() const;

    std::vector<uint8_t> encode() const;
};

packet create_ping();
packet create_pong();
packet create_msg(const std::string& msg);

}

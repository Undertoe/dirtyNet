#pragma once

#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <optional>
#include <stdint.h>

namespace tcp_packet
{

enum class type : uint32_t{
    ping, pong, greeting, invalid,
};

std::string type_as_string(type);


struct header{
    type _type{type::invalid};

    header() = default;
    header(const char*, size_t);

    bool encode(char*, size_t) const;

    // std::vector<uint8_t> encode() const;
};

struct packet{
    header hdr; // NOT INCLUDING THE HEADER LENGTH
    uint32_t datalen{0};
    std::string msg;
    bool validPacket{true};

    packet() = delete;
    packet(type t);
    packet(char* , size_t );
    packet(const std::string& msg);

    std::string type_string() const;

    bool encode(char*, size_t) const;
};

std::optional<packet> parse_packet(char*, size_t);

packet create_ping();
packet create_pong();
packet create_msg(const std::string& msg);

}

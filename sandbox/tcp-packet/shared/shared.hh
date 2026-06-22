#pragma once

#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <span>
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

    bool encode(char*, size_t) const;

    // std::vector<uint8_t> encode() const;
};

struct packet{
    header hdr; // NOT INCLUDING THE HEADER LENGTH
    uint32_t datalen{0};
    std::string msg;
    bool validPacket{true};

    packet() = delete;
    packet(const packet&) = default;
    packet& operator=(const packet&) = default;
    packet(packet&&) = default;
    packet& operator=(packet&&) = default;
    packet(type t);
    packet(const std::string& msg);

    std::string type_string() const;

    bool encode(char*, size_t) const;
};

enum class parse_status{
    complete, incomplete, invalid,
};

struct parse_result{
    parse_status status {parse_status::invalid};
    std::optional<packet> pkt;
    size_t bytes_consumed{0};
};

parse_result parse_packet(std::span<char>);

packet create_ping();
packet create_pong();
packet create_msg(const std::string& msg);

}

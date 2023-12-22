#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.hh"

namespace dirtyNet
{
class ipv4
{
public:
    ipv4(uint32_t);
    ipv4(std::string_view);

    bool Valid() const;
    in_addr Address() const;
    std::string IP() const;


private:
    in_addr _addr{0};
    bool _valid{false};

};

} // end namespace dirtyNet
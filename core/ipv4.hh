#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <functional>

#include "util.hh"

namespace dirtyNet
{
class ipv4
{
public:
    ipv4(uint32_t);
    ipv4(in_addr);
    ipv4(std::string_view);

    bool Valid() const;
    in_addr Address() const;
    std::string_view String() const;


private:
    in_addr _addr{0};
    std::array<char,INET_ADDRSTRLEN> _addrStr{'\0'}; 
    bool _valid{false};
};


} // end namespace dirtyNet


bool operator==(const in_addr& lhs, const in_addr& rhs);
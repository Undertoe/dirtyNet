#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

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
    mutable std::array<char, INET_ADDRSTRLEN> _addrStr{'\0'};
    mutable bool _validString{false};
};

}  // end namespace dirtyNet

bool operator==(const in_addr& lhs, const in_addr& rhs);
#include "ipv4.hh"


dirtyNet::ipv4::ipv4(uint32_t ipInt) : _valid{true}
{
    _addr.s_addr = ipInt;
}


dirtyNet::ipv4::ipv4(std::string_view ipString)
{
    _valid = (inet_pton(AF_INET, std::string(ipString).c_str(), &_addr) == 1);
}


bool
dirtyNet::ipv4::Valid() const
{
    return _valid;
}

in_addr
dirtyNet::ipv4::Address() const
{
    return _addr;
}



bool operator==(const in_addr& lhs, const in_addr& rhs)
{
    return lhs.s_addr == rhs.s_addr;
}
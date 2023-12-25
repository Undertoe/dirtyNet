#include "ipv6.hh"


dirtyNet::ipv6::ipv6(std::string_view sv)
{
    _valid = inet_pton(AF_INET6, std::string(sv).c_str(), &_addr) == 1;
    for(int i = 0; i < sv.size() && i < _addrStr.size(); i ++)
    {
        _addrStr[i] = sv[i];
    }
}

dirtyNet::ipv6::ipv6(const in6_addr& addr)
{
    _addr = addr;
    _valid = inet_ntop(AF_INET6, &_addr, &_addrStr[0], INET6_ADDRSTRLEN);
}


bool
dirtyNet::ipv6::Valid() const
{
    return _valid;
}


in6_addr
dirtyNet::ipv6::Address() const
{
    return _addr;
}

bool operator==(const in6_addr& lhs, const in6_addr& rhs)
{
    for(int i = 0; i < 4; i++)
    {
        if(lhs.__in6_u.__u6_addr32[i] != rhs.__in6_u.__u6_addr32[i])
        {
            return false;
        }
    }
    return true;
}

std::string_view
dirtyNet::ipv6::String() const
{
    return std::string_view(_addrStr.data(), dirtyNet::find_end_char(_addrStr, '\0'));
}
#include "ipv6.hh"


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
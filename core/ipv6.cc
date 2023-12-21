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
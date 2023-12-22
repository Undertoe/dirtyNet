#include "ip.hh"



dirtyNet::ip::ip(ipv4&& ip) : _ip(ip)
{

}

dirtyNet::ip::ip(ipv6&& ip) : _ip(ip)
{

}


bool
dirtyNet::ip::Valid() const
{
    if(const auto& v = std::get_if<ipv4>(&_ip); v)
    {
        return v->Valid();
    }
    else if(const auto & v  = std::get_if<ipv6>(&_ip); v)
    {
        return v->Valid();
    }
    return false;
}
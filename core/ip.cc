#include "ip.hh"



dirtyNet::ip::ip(ipv4&& ip) : _ip(ip)
{

}

dirtyNet::ip::ip(ipv6&& ip) : _ip(ip)
{

}

dirtyNet::ip::ip(std::string_view ipString) : _ip(ipv4(ipString))
{
    if(ipString.find_first_of(":") != std::string_view::npos)
    {
        _ip = ipv6(ipString);
    }
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

dirtyNet::ip::AddressType
dirtyNet::ip::Address() const
{
    if(const auto& v = std::get_if<ipv4>(&_ip); v)
    {
        return v->Address();
    }
    else if(const auto & v  = std::get_if<ipv6>(&_ip); v)
    {
        return v->Address();
    }
    return {};
}

dirtyNet::ip_type
dirtyNet::ip::Type() const
{
    if(std::holds_alternative<ipv4>(_ip))
    {
        return ip_type::ipv4;
    }
    return ip_type::ipv6;
}
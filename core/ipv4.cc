#include "ipv4.hh"


dirtyNet::ipv4::ipv4(uint32_t ipInt) 
{
    _addr.s_addr = ipInt;
    // std::cout << "[" << std::string_view(_addrStr.begin(), _addrStr.end()) << "]" << std::endl;
    _valid = inet_ntop(AF_INET, &_addr, &_addrStr.front(), INET_ADDRSTRLEN);
    // std::cout << "[" << std::string_view(_addrStr.begin(), _addrStr.end()) << "]" << std::endl;
}

dirtyNet::ipv4::ipv4(in_addr ip) : _addr(ip)
{
    _valid = inet_ntop(AF_INET, &_addr, &_addrStr.front(), INET_ADDRSTRLEN);

}


dirtyNet::ipv4::ipv4(std::string_view ipString) 
{
    _valid = (inet_pton(AF_INET, std::string(ipString).c_str(), &_addr) == 1);
    std::memcpy(&_addrStr.front(), &ipString[0], INET_ADDRSTRLEN);
    // for(int i = 0; i < ipString.length() && i < _addrStr.size(); i ++)
    // {
    //     _addrStr[i] = ipString[i];
    // }
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

std::string_view
dirtyNet::ipv4::String() const
{
    return std::string_view(_addrStr.data(), dirtyNet::find_end_char(_addrStr, '\0'));
}



bool operator==(const in_addr& lhs, const in_addr& rhs)
{
    return lhs.s_addr == rhs.s_addr;
}
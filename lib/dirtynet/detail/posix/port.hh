#pragma once

#include <netinet/in.h>
#include <cstdint>
#include <stdint.h>

#include <compare>
#include <string>

namespace dirtynet::detail::posix {

class port
{
public: 
    
    static port from_native(uint16_t p)
    {
        return port(p, true);
    }

    

    port(uint16_t p) : _port(htons(p)), _local(p)
    {

    }
    
    std::string to_string() const 
    {
        return std::to_string(_local);
    }

    uint16_t posix() const{
        return _port;
    }

    
    bool operator ==(const port& other) const
    {
        return _local == other._local;
    }

    std::strong_ordering operator <=> (const port& other)
    {
        return _local <=> other._local;
    }


private:
    port(uint16_t port, bool native_flag) : _port(port) , _local(ntohs(port))
    {

    }

    uint16_t _port;
    uint16_t _local;

};
}
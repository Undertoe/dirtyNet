#pragma once

#include <string_view>
#include <charconv>
#include <expected>
#include <format>

#include "ip.hh"



namespace dirtyNet
{

    // enum class endpoint_error_e
    // {
    //     invalid_endpoint_format,
    //     invalid_port_number_string,
    // };

    // std::string endpoint_error_string(dirtyNet::endpoint_error_e e);

    class endpoint{
    public:
        static std::expected<endpoint, std::string> parse_endpoint(std::string_view);

        endpoint(std::string_view, std::string_view);
        endpoint(std::string_view, uint16_t);
        endpoint(in_addr, uint16_t);
        endpoint(in6_addr, uint16_t);

        std::variant<in_addr,in6_addr> Address() const;
        std::string_view IpString() const;
        
        uint16_t Port() const;
        uint16_t NetPort() const;
        
    private:
        ip _ip;
        uint16_t _port;
        uint16_t _netPort;

    };
}
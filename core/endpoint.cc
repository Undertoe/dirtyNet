#include "endpoint.hh"

// std::string endpoint_error_string(dirtyNet::endpoint_error_e e)
// {
//     using namespace dirtyNet;
//     switch(e)
//     {
//         case endpoint_error_e::invalid_endpoint_format:
//             return "Invalid Endpoint format.  Expected <ip>:<port> format (such as 192.168.0.1:8080)";
//         case endpoint_error_e::invalid_port_number_string:
//             return "Invalid number string caught";
//     }
// }

std::expected<dirtyNet::endpoint, std::string> dirtyNet::endpoint::parse_endpoint(std::string_view endpoint_str)
{
    auto colon_pos = endpoint_str.find_last_of(':');
    if (colon_pos == std::string_view::npos) {
        return std::unexpected(std::format("invalid ip:port string caught {}.  expecting format <ip>:<port>", endpoint_str));
    }

    std::string_view ip_str = endpoint_str.substr(0, colon_pos);
    std::string_view port_str = endpoint_str.substr(colon_pos + 1);

    if (ip_str == "localhost") {
        ip_str = "127.0.0.1";
    }

    if (ip_str.front() == '[' && ip_str.back() == ']') {
        ip_str = ip_str.substr(1, ip_str.size() - 2);
    }
    try
    {
        return dirtyNet::endpoint(ip_str, port_str);
    }
    catch(const std::invalid_argument& e)
    {  
        return std::unexpected(std::format("Invalid port number port {}\n", port_str));
    }
}

dirtyNet::endpoint::endpoint(std::string_view ip_str, std::string_view port_str)
    : _ip(ip_str)
{
    uint16_t port = 0;
    auto [ptr, ec] = std::from_chars(port_str.data(), port_str.data() + port_str.size(), port);
    if (ec != std::errc()) {
        throw std::invalid_argument("Invalid port number"); 
    }
    _port = port;
    _netPort = htons(port);
}

dirtyNet::endpoint::endpoint(std::string_view ip_str, uint16_t port)
    : _ip(ip_str), _port(port), _netPort(htons(port))
{
}

dirtyNet::endpoint::endpoint(in_addr addr, uint16_t port)
    : _ip(addr), _port(port), _netPort(htons(port))
{
}

dirtyNet::endpoint::endpoint(in6_addr addr, uint16_t port)
    : _ip(addr), _port(port), _netPort(htons(port))
{
}

std::variant<in_addr,in6_addr>
dirtyNet::endpoint::Address() const
{
    return _ip.Address();
}

std::string_view
dirtyNet::endpoint::IpString() const 
{
    return _ip.String();
}

uint16_t
dirtyNet::endpoint::Port() const
{
    return _port;
}


#include "endpoint.hh"


dirtyNet::endpoint dirtyNet::endpoint::parse_endpoint(std::string_view endpoint_str)
{
    auto colon_pos = endpoint_str.find_last_of(':');
    if (colon_pos == std::string_view::npos) {
        throw std::invalid_argument("Invalid endpoint format, expecting <ip>:<port> format");
    }

    std::string_view ip_str = endpoint_str.substr(0, colon_pos);
    std::string_view port_str = endpoint_str.substr(colon_pos + 1);

    if (ip_str == "localhost") {
        ip_str = "::1";
    }

    if (ip_str.front() == '[' && ip_str.back() == ']') {
        ip_str = ip_str.substr(1, ip_str.size() - 2);
    }

    return dirtyNet::endpoint(ip_str, port_str);
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
}

dirtyNet::endpoint::endpoint(std::string_view ip_str, uint16_t port)
    : _ip(ip_str), _port(port)
{
}

dirtyNet::endpoint::endpoint(in_addr addr, uint16_t port)
    : _ip(addr), _port(port)
{
}

dirtyNet::endpoint::endpoint(in6_addr addr, uint16_t port)
    : _ip(addr), _port(port)
{
}

std::variant<in_addr,in6_addr>
dirtyNet::endpoint::Address() const
{
    return _ip.Address();
}

uint16_t
dirtyNet::endpoint::Port() const
{
    return _port;
}
#pragma once


#include "dirtynet/detail/native_udpsocket.hh"
#include "endpoint.hh"
#include "detail/native_endpoint.hh"
#include "dirtynet/endpoint.hh"
#include "error/udp.hh"
#include <algorithm>
#include <span>
#include <cstddef>

#include <expected>
#include <vector>


namespace dirtynet{

struct udp;

struct udp_datagram
{
    std::vector<std::byte> bytes;
    endpoint sender;
    bool truncated;

    udp_datagram(const std::vector<std::byte>& b, const endpoint& e, bool t) : 
        bytes(b), sender(e), truncated(t) {}

    udp_datagram(std::vector<std::byte>&& b, endpoint&& e, bool t) : 
        bytes(b), sender(e), truncated(t) {}

    udp_datagram() = delete;
    ~udp_datagram() = default;
    
    udp_datagram(const udp_datagram&) = default;
    udp_datagram(udp_datagram&&) = default;

    udp_datagram& operator=(const udp_datagram&) = default;
    udp_datagram& operator=(udp_datagram&&) = default;
};

class udp_socket
{
public:

    static std::expected<udp_socket, udp_open_error> open()
    {
        auto mNative = detail::native::udp_socket::open();
        if(!mNative)
        {
            return std::unexpected<udp_open_error>(mNative.error());
        }

        return udp_socket(std::move(*mNative));
    }

    static std::expected<udp_socket, udp_bind_error> bind(const dirtynet::endpoint& endpoint)
    {
        auto mNative = detail::native::udp_socket::bind(detail::endpoint_native_access::get(endpoint));
        if(!mNative)
        {
            return std::unexpected<udp_bind_error>(mNative.error());
        }

        return udp_socket(std::move(*mNative));
    }


    std::expected<int, dirtynet::udp_send_error> send_to(const endpoint& target, const std::span<const std::byte>& bytes)
    {
        return _native.send_to(detail::endpoint_native_access::get(target), bytes);
    }

    std::expected<udp_datagram, dirtynet::udp_recv_error> receive()
    {
        auto r = _native.receive();
        if(!r)
        {
            return std::unexpected<dirtynet::udp_recv_error>(r.error());
        }
        return udp_datagram{r->bytes, detail::endpoint_native_access::from_native(r->sender), r->truncated};
    }


private: 

    udp_socket(detail::native::udp_socket&& other) : _native(std::move(other)) {}

    

    detail::native::udp_socket _native;

    friend struct udp;

};

}
#pragma once

#include <cstddef>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>

#include <span>
#include <expected>
#include <vector>


#include <dirtynet/error/udp.hh>
#include "endpoint.hh"


namespace dirtynet::detail::posix {

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
    static std::expected<udp_socket, dirtynet::udp_open_error> open()
    {
        auto fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(fd < 0)
        {
            return std::unexpected<dirtynet::udp_open_error>(dirtynet::udp_open_error::error);
        }
        return udp_socket{fd};

    }

    static std::expected<udp_socket, dirtynet::udp_bind_error> bind(const endpoint& ep)
    {
        auto fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(fd < 0)
        {
            return std::unexpected<dirtynet::udp_bind_error>(dirtynet::udp_bind_error::error);
        }
        udp_socket sock{fd};
        auto [addr, len] = ep.addr_info();
        auto bound = ::bind(fd, (sockaddr*)addr, len);
        if(bound < 0)
        {
            return std::unexpected<dirtynet::udp_bind_error>(dirtynet::udp_bind_error::error);
        }
        return sock;
    }

    udp_socket(udp_socket&& other)
    {
        _fd = other._fd;
        other._fd = -1;
    }

    udp_socket& operator=(udp_socket&& other)
    {
        // close our socket if it was already opened.
        if(_fd > -1)
        {
            close(_fd);
        }
        _fd = other._fd;
        other._fd = -1;
        return *this;
    }

    ~udp_socket()
    {
        // fd @ -1 is an invalid fd sentinal value.
        if(_fd >= 0)
        {
            close(_fd);
        }
    }

    std::expected<int, dirtynet::udp_send_error> send_to(const endpoint& target, const std::span<const std::byte>& bytes)
    {
        // if(bytes.size() == 0)
        // {
        //     return std::unexpected<dirtynet::udp_send_error>(udp_send_error::error);
        // }
        auto [to, length] = target.addr_info();
        auto n = ::sendto(_fd, bytes.data(), bytes.size(), MSG_CONFIRM, to, length);
        if(n < 0)
        {
            return std::unexpected<dirtynet::udp_send_error>(udp_send_error::error);
        }
        return n;
    }

    std::expected<udp_datagram, dirtynet::udp_recv_error> receive()
    {
        if(_fd < 0)
        {
            return std::unexpected<dirtynet::udp_recv_error>(udp_recv_error::error);
        }
        sockaddr_in rec_client{0};
        socklen_t len {sizeof(rec_client)};
        auto n = ::recvfrom(_fd, _bytes.data(), _bytes.size(), MSG_WAITALL, (sockaddr*)&rec_client, &len);

        if(n < 0)
        {
            return std::unexpected<dirtynet::udp_recv_error>(udp_recv_error::error);
        }
        std::vector<std::byte> bytes(n);
        std::memcpy(bytes.data(), _bytes.data(), n);
        return udp_datagram(bytes, endpoint(rec_client), false);
    } 
    

    // used in the future to clean up thread contexts in future revisions.
    bool kill()
    {
        return true;
    }

    std::optional<port> get_port() const
    {
        sockaddr_in addr{};
        socklen_t len = sizeof(addr);
        if(::getsockname(_fd, (sockaddr*) &addr, &len) == -1)
        {
            return std::nullopt;
        }
        return port::from_native(addr.sin_port);
    }


private:
    explicit udp_socket(int fd) : _fd(fd) {} 
    static constexpr size_t _max_ipv4_packet_size = 65507;
    std::vector<std::byte> _bytes{_max_ipv4_packet_size};

    int _fd{-1};


};

}
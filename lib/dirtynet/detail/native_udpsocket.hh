#pragma once

#include "platform.hh"


#if defined(DIRTYNET_PLATFORM_POSIX)

#include "posix/udp_socket.hh"

namespace dirtynet::detail::native {
    using udp_datagram = posix::udp_datagram;
    using udp_socket = posix::udp_socket;
}

#endif
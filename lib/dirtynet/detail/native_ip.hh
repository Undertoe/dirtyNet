#pragma once

#include "platform.hh"

#if defined(DIRTYNET_PLATFORM_POSIX)

#include "posix/ip.hh"


// Platform-neutral native ip storage for ipv4 & ipv6
namespace dirtynet::detail::native {
    using ipv4 = posix::ipv4;
    using ipv4_internal = posix::ipv4::internal_t;
    using ipv6 = posix::ipv6;
    using ip = posix::ip;
}


#endif
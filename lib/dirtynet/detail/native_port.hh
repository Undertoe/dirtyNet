#pragma once

#include "platform.hh"

#if defined(DIRTYNET_PLATFORM_POSIX)

#include "posix/port.hh"


// Platform-neutral native ip storage for ipv4 & ipv6
namespace dirtynet::detail::native {
    using port = posix::port;

}


#endif
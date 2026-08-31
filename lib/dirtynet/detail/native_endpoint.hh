#pragma once

#include "platform.hh"
#include <cstddef>
#include <variant>

#if defined(DIRTYNET_PLATFORM_POSIX)

#include "posix/endpoint.hh"


namespace dirtynet::detail::native {
using endpoint = posix::endpoint;
using socket_length_t = endpoint::socket_length_t;
using endpoint_internal_v4 = endpoint::socket_addr_v4_t;
using endpoint_internal_v6 = endpoint::socket_addr_v6_t; 
}

#endif

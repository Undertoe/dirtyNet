#pragma once

#include "platform.hh"

#if defined(DIRTYNET_PLATFORM_POSIX)

#include "posix/endpoint.hh"


namespace dirtynet::detail::native {
using endpoint = posix::endpoint;
}

#endif

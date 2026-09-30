#pragma once


#if defined (_WIN32)

#define DIRTYNET_PLATFORM_WINDOWS 1

#elif defined(__unix__) || defined(__APPLE__)

#define DIRTYNET_PLATFORM_POSIX 1

#else

#error "Unsupported dirtynet platform"

#endif

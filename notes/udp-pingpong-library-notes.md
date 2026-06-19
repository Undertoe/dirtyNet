# UDP Ping-Pong Library Notes

These notes capture design pressure discovered while writing the basic UDP
ping-pong sandbox. The sandbox should stay simple and raw enough to expose the
boilerplate, while these notes describe what dirtyNet should eventually absorb.

## Immediate Boilerplate

- `sockaddr_in` setup is repetitive and easy to get subtly wrong.
- Port values need explicit host/network byte-order handling.
- Raw socket file descriptors need clear ownership, cleanup, and error paths.
- `bind`, `sendto`, and `recvfrom` all expose C-style pointer and length pairs
  that should not leak throughout user code.

## Endpoint Shape

The library should provide an endpoint/address type that can construct and own
the platform socket-address representation.

Expected responsibilities:

- Build a UDP IPv4 endpoint from host/address and port inputs.
- Serialize into the `sockaddr` representation needed by POSIX calls.
- Keep the address length paired with the address storage.
- Make byte-order conversion part of construction rather than caller ceremony.
- Leave room for IPv6 without making the first UDP milestone too broad.

Open naming question: whether this should be called `endpoint`,
`udp_endpoint`, `socket_address`, or something more specific.

## UDP Socket Shape

The library should provide a `udp_socket` type that owns its socket file
descriptor and exposes UDP operations at a higher level than POSIX.

Expected responsibilities:

- Create and close the socket descriptor with RAII ownership.
- Prevent accidental descriptor leaks and double-close behavior.
- Offer a bindable option, likely `bind(endpoint)` or a constructor/factory for
  bound sockets.
- Support `send_to(endpoint, bytes)` for datagrams.
- Support `receive_from(buffer)` returning both bytes received and sender
  endpoint information.
- Leave room for typed `send` and `receive` APIs that can move data into and
  out of user-selected object types.
- Preserve access to meaningful error information instead of returning raw
  negative syscall values alone.

## Typed Data Boundary

The raw socket layer deals in bytes, but user code will often want to send and
receive meaningful objects. The library should eventually provide a clean way to
connect those two worlds without hard-coding one serialization format.

Possible direction:

- Keep byte-oriented `send_to` and `receive_from` as the fundamental operations.
- Add template `send` and `receive` conveniences for object-shaped data.
- Let the caller choose or define how an object becomes bytes and how bytes
  become an object.
- Consider a codec/serializer concept or traits boundary rather than requiring
  every object to expose the same member functions.
- Make receive APIs account for both the decoded object and the sender endpoint.

Open questions:

- Should typed UDP send/receive be part of `udp_socket`, a wrapper over it, or a
  separate codec layer?
- Should receive construct an object, fill an existing object, or support both?
- How should decode failures be represented alongside socket errors?
- What is the lowest-friction first form for plain structs and string-like
  messages?

## First Abstraction Boundary

The first useful UDP layer should probably wrap ownership and address handling,
not protocol policy.

Good first slice:

- `endpoint` or `udp_endpoint`
- `udp_socket`
- blocking `bind`
- blocking `send_to`
- blocking `receive_from`

Defer for later:

- generalized typed serialization
- retries and timeouts
- nonblocking mode
- polling/epoll integration
- packet framing
- multi-peer session management
- TCP-generalized abstractions

## Sandbox Feedback Loop

Keep using the UDP ping-pong sandbox as a raw reference while designing the
library API. When a line of sandbox code feels like mechanical setup rather than
the actual behavior being studied, consider whether it belongs behind an
endpoint or socket abstraction.

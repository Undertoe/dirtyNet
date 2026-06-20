# Learning C Sockets Notes

This is the working notebook for C socket exercises, starting with the UDP
ping-pong sandbox.

Use this file for anything worth remembering while reading man pages, writing
small socket programs, debugging behavior, or noticing patterns that dirtyNet
may eventually wrap. The main goal right now is learning the raw C/POSIX socket
surface before turning it into library code.

## Current Exercise

UDP 1-to-1 ping-pong.

What this exercise is for:

- Learn the minimum syscall flow for a UDP server and client.
- See which parts are real networking behavior and which parts are ceremony.
- Get comfortable with socket addresses, byte order, buffers, and error paths.
- Keep the sandbox direct enough that the POSIX calls are still visible.

## Running Notes

Add new observations here as they come up.

- `socket`, `bind`, `sendto`, and `recvfrom` form the basic UDP loop.
- UDP does not create a persistent connection for basic send/receive flows.
- A server usually binds to a local address and port so clients have a stable
  destination.
- A client can often send without an explicit bind; the OS can assign an
  ephemeral local port.
- `recvfrom` can report both the payload and the address that sent it.
- `sendto` needs the destination address every time unless the socket is
  connected with `connect`.

## Vocabulary

- socket descriptor: integer handle returned by `socket`.
- datagram: one UDP message.
- local address: address and port the socket is bound to on this machine.
- remote address: address and port for the peer.
- network byte order: big-endian byte order used in socket address fields.
- host byte order: the CPU's native byte order.
- `sockaddr_in`: IPv4 socket address structure.
- `sockaddr`: generic address pointer type used by socket syscalls.
- `socklen_t`: length type paired with socket address structures.

## Socket Types

`SOCK_STREAM`:

- Connection-oriented byte stream.
- This is the usual socket type for TCP.
- A connection has to be established and maintained between peers.
- The socket preserves byte order, but it does not preserve application-level
  message boundaries.
- The application has to define its own structure for finding message or packet
  boundaries inside the stream.

`SOCK_DGRAM`:

- Datagram/message-oriented socket.
- This is the usual socket type for UDP.
- A connection is not required for the basic send/receive path.
- Each datagram is preserved as a single message entity rather than merged into
  a continuous stream.

`SOCK_SEQPACKET`:

- Connection-oriented message socket.
- This is a connection-oriented datagram-style model: it requires a connection,
  but it preserves each message as an isolated event instead of exposing one
  continuous byte stream.
- This is not the normal TCP socket type. TCP uses `SOCK_STREAM`.
- Commonly used with Unix domain sockets for local IPC:
  `socket(AF_UNIX, SOCK_SEQPACKET, 0)`.
- Can also be used with SCTP when the system supports it:
  `socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)`.
- Useful as a contrast with `SOCK_STREAM`, because it keeps reliable ordered
  delivery while still preserving message boundaries.

Stream vs. message boundaries:

- With `SOCK_STREAM`, sending `hello` and then `world` can be received as
  `helloworld`, `hello` then `world`, or smaller chunks such as `he`, `llowor`,
  and `ld`.
- With datagram or sequence-packet sockets, `hello` and `world` remain separate
  receive-side messages.
- Stream sockets need application-level framing, such as `[4 bytes length]`
  followed by `[payload bytes]`, so the receiver knows when a complete message
  has arrived.

## C Socket Mechanics

Address setup:

- `sockaddr_in` setup is repetitive and easy to get subtly wrong.
- `sin_family` must match the address family, such as `AF_INET` for IPv4.
- Port values need explicit host/network byte-order handling with `htons` and
  `ntohs`.
- IPv4 address values need conversion from text to binary representation, such
  as with `inet_pton`.
- POSIX calls often want a `sockaddr*`, so IPv4-specific addresses usually get
  cast at the call boundary.
- Keep the address length paired with the address object.

Descriptor and error handling:

- Raw socket descriptors need clear ownership, cleanup, and error paths.
- `socket`, `bind`, `sendto`, and `recvfrom` use negative return values to
  signal errors.
- `errno` has to be checked close to the failing call before anything else can
  overwrite it.
- Every successful descriptor creation needs a matching `close`.

Buffer handling:

- The raw socket layer deals in bytes.
- `sendto` and `recvfrom` expose C-style pointer and length pairs.
- UDP preserves datagram boundaries, but fixed-size receive buffers can still
  truncate messages.
- The return value from `recvfrom` is the number of payload bytes received.

## Questions To Chase

- What are the exact differences between binding to `INADDR_ANY`, loopback, and
  a specific local interface address?
- What happens when a UDP receive buffer is smaller than the incoming datagram?
- When is UDP `connect` useful, and what behavior changes after calling it?
- Which errors are common for UDP `sendto` and `recvfrom` in local examples?
- What is the cleanest way to print peer addresses while debugging?
- How should the examples handle Ctrl-C and cleanup while staying simple?

## Man Pages And References

Useful pages while working through the exercises:

- `man 2 socket`
- `man 2 bind`
- `man 2 sendto`
- `man 2 recvfrom`
- `man 2 connect`
- `man 2 close`
- `man 3 inet_pton`
- `man 3 htons`

## Exercise Log

Use this section to add dated or task-specific notes.

### UDP Ping-Pong

- Server shape: create UDP socket, bind local address, wait with `recvfrom`,
  reply with `sendto`.
- Client shape: create UDP socket, send message to server address with
  `sendto`, wait for reply with `recvfrom`.
- Main learning target: understand all address setup and syscall arguments
  before hiding them behind helper types.

## Later dirtyNet Library Ideas

These are not the immediate learning target. They are design pressure noticed
while writing raw socket exercises.

### Endpoint Shape

The library may eventually provide an endpoint/address type that can construct
and own the platform socket-address representation.

Possible responsibilities:

- Build a UDP IPv4 endpoint from host/address and port inputs.
- Serialize into the `sockaddr` representation needed by POSIX calls.
- Keep the address length paired with the address storage.
- Make byte-order conversion part of construction rather than caller ceremony.
- Leave room for IPv6 without making the first UDP milestone too broad.

Open naming question: whether this should be called `endpoint`,
`udp_endpoint`, `socket_address`, or something more specific.

### UDP Socket Shape

The library may eventually provide a `udp_socket` type that owns its socket file
descriptor and exposes UDP operations at a higher level than POSIX.

Possible responsibilities:

- Create and close the socket descriptor with RAII ownership.
- Prevent accidental descriptor leaks and double-close behavior.
- Offer a bindable option, likely `bind(endpoint)` or a constructor/factory for
  bound sockets.
- Support `send_to(endpoint, bytes)` for datagrams.
- Support `receive_from(buffer)` returning both bytes received and sender
  endpoint information.
- Preserve access to meaningful error information instead of returning raw
  negative syscall values alone.

### Typed Data Boundary

The raw socket layer deals in bytes, but user code will often want to send and
receive meaningful objects. A future library layer should connect those two
worlds without hard-coding one serialization format.

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

### First Abstraction Boundary

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

Keep using the UDP ping-pong sandbox as a raw reference while learning the
socket API. When a line of sandbox code feels like mechanical setup rather than
the actual behavior being studied, note it here before deciding whether it
belongs behind an endpoint or socket abstraction.

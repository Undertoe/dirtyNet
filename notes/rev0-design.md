# dirtyNet Revision 0 Design

## Status And Purpose

This document captures the current design direction for revision 0 of dirtyNet.
It is a starting point for further design work, not a frozen API specification.
Code examples communicate responsibilities and relationships; they do not commit
the project to exact names, signatures, constraints, or implementation details.

The `rev0_design` branch is documentation-only. Revision 0 implementation work
will happen later and should be based on intentionally reviewed design notes.

## Project Goal

dirtyNet is intended to be a generic C++ networking library. Applications such
as a market-data subscriber, packet-monitoring tool, server, or client are
validation targets for the library; they do not define the library's protocol
or architecture.

The library should make common networking work approachable for newer C++
developers while retaining lower-level operations and future customization
points for advanced users.

Two project-wide constraints apply:

1. The public dirtyNet API should be operating-system agnostic. Code using the
   ordinary API should have the same source-level shape on POSIX and Windows,
   and endpoints created on either platform must interoperate over standard
   network protocols.
2. Revision 0 implementation work will focus on POSIX. Windows support is a
   planned revision 1 concern. POSIX-specific details must therefore be kept
   behind small, identifiable native boundaries that can be replaced or
   conditionally implemented later.

Operating-system agnostic does not mean that internal representations, native
handle types, polling systems, or native error codes must be identical. It
means that the public behavioral model remains stable.

## Design Principles

### Use Types To Express Real Semantic Differences

The type system should prevent invalid operations when two roles have genuinely
different capabilities. TCP connection acceptance and connected-stream I/O are
different roles and should not be methods on one unrestricted public socket
type.

Not every runtime state needs its own type. Explicitly bound and loose UDP
sockets currently appear to have the same useful operation set, so revision 0
does not yet need separate public types for them.

### Event-Driven Usage Is The Preferred High-Level Path

The preferred application-facing workflow reacts to incoming connections or
data through user-provided functions or lambdas. The library should manage the
repeated wait/receive/dispatch loop according to a selected execution strategy.

Direct read, write, send, receive, and accept operations must still exist. They
are the behavioral foundation beneath the event-driven layer and remain useful
for explicit control, testing, and specialized workflows.

### Preserve Transport Semantics

UDP and TCP can share byte-storage conventions and internal native-handle
ownership, but their public operations should not falsely imply identical
behavior.

- UDP preserves datagram boundaries and can report a sender endpoint for every
  received datagram.
- TCP is a connected byte stream. Reads expose available chunks, not complete
  application messages.
- A TCP server-side acceptance resource produces connections; it does not read
  application bytes directly.

### Prefer Valid Constructed Resources

Revision 0 will use named, `std::expected`-returning factory operations for
fallible socket creation. Public constructors should not create invalid socket
objects that callers must remember to inspect.

Throwing convenience constructors may be explored after the expected-based
path is stable. They are not part of the revision 0 baseline.

### Make Cross-Platform Replacement Seams Obvious

Revision 0 does not need a speculative, fully generalized Windows backend. It
does need centralized names and boundaries so POSIX-native code can be found
and replaced without redesigning the public endpoint and socket APIs.

## Architectural Layers

The current high-level architecture is:

```text
Application protocols and application handlers
                         |
Preferred event-driven dirtyNet API
  on_receive / on_connection and managed operations
                         |
Direct transport operations
  read / write / send_to / receive_from / accept
                         |
Transport-specific public resource types
  udp_socket / tcp_connection_handler / tcp_connection
                         |
Shared internal ownership and backend utilities
  native socket handle / native endpoint / native errors
                         |
Revision 0 POSIX implementation
  socket / bind / listen / accept / connect / send / receive
```

Application framing and packet meaning remain above the core transport layer.
dirtyNet may eventually provide optional framing or typed-I/O helpers, but its
source of truth is byte-oriented I/O.

## Address Foundation

### `port`

`port` is a distinct value type rather than an unlabelled integer.

Current responsibilities:

- Store the application-facing port value in host byte order.
- Keep port `0` valid for operating-system-selected ephemeral ports.
- Provide comparison and formatting suitable for ordinary numeric use.
- Own or mediate the conversion to and from the representation required by the
  native networking boundary.
- Prevent routine application code from manually calling byte-order conversion
  functions.

The internal representation is expected to be a fixed-width unsigned value.
Exact constructor validation, parsing helpers, and naming remain open.

### `ipv4` And `ipv6`

`ipv4` and `ipv6` are distinct concrete address-family types. They own parsing,
formatting, family-specific special values, and access to their native binary
representations.

Conceptual facilities include:

```cpp
ipv4{"127.0.0.1"};
ipv6{"::1"};

ipv4::localhost();
ipv6::localhost();

address.string();
address.native();
```

For revision 0:

- Text construction parses immediately into binary representation.
- The binary address is the value's identity.
- `string()` formats the binary address on demand.
- The originally supplied string is not retained.
- No lazy string cache or mutex is required.

A revision 1 or later `ip_string` facility may preserve caller-supplied text,
cache generated presentation text, provide canonical/compressed formatting,
and synchronize lazy initialization. This is explicitly outside the MVP
because it primarily improves diagnostics and does not justify complicating
the foundational value types yet.

`native()` should be a zero-allocation, non-throwing view or reference into
already stored native binary state. It is an implementation/interoperability
escape hatch, not the ordinary application-facing representation.

### `ip`

`ip` is the family-erased address type and contains either `ipv4` or `ipv6`,
conceptually through a variant.

```text
ip
`-- variant
    |-- ipv4
    `-- ipv6
```

The desired comparison boundaries are:

```cpp
ipv4 == ipv4; // valid
ipv6 == ipv6; // valid
ip == ip;     // valid

ipv4 == ipv6; // intentionally unavailable
ipv4 == ip;   // intentionally unavailable
ipv6 == ip;   // intentionally unavailable
```

At the `ip` level, equality is based on address family and binary address. IPv4
loopback and IPv6 loopback are both meaningful loopback destinations, but they
remain different concrete network addresses.

First-class defaults should exist:

```cpp
ip::localhost();             // defaults to IPv4
ip{ipv6::localhost()};       // explicit IPv6 override
```

A runtime family-taking convenience may be considered later. A template flag
is not currently necessary because `ip` already represents a runtime family
choice.

### `endpoint`

`endpoint` is the primary network-location value used by socket operations. It
combines an `ip` and a `port`.

```text
endpoint = ip + port
```

It is used as appropriate by bind, connect, send-to, local-endpoint reporting,
remote-endpoint reporting, and sender reporting.

Numeric construction remains explicit about address family:

```cpp
endpoint{ipv4{"127.0.0.1"}, port{8080}};
endpoint{ipv6{"::1"}, port{8080}};
endpoint{ip::localhost(), port{8080}};
```

The concrete-family overloads are conveniences that wrap their input in `ip`;
the endpoint's platform-neutral semantic address remains `ip + port`.

An endpoint string construction path may perform operating-system name
resolution:

```cpp
endpoint{"example.com", port{443}};
```

The revision 0 resolution rule is intentionally simple:

- Treat explicit `ipv4` and `ipv6` construction as numeric parsing.
- Treat the endpoint hostname path as OS-backed resolution.
- Select the first address returned by the OS resolver.
- Store the resolved binary `ip` and `port`, not the hostname.
- Do not promise identical address-family selection across systems because OS
  resolver order and local configuration can differ.

The precise fallible construction shape for hostname resolution remains open,
especially because constructors cannot return `std::expected`.

### Native Endpoint Representation

Socket operations ultimately need the complete native address, including
family, IP, port, and length. An immutable endpoint can eagerly construct and
cache this native representation once.

Conceptually:

```text
endpoint
|-- ip
|-- port
`-- native_endpoint
    |-- native address storage
    `-- native address length
```

`endpoint::native()` should return a lightweight, non-owning view into this
ready representation. On POSIX, the implementation can wrap address storage
and its associated length. Windows revision 1 can replace the internals while
preserving the high-level endpoint contract.

The inverse path is also required:

```text
OS receive fills native endpoint storage
                    |
                    v
endpoint::from_native(native_endpoint)
                    |
                    v
ordinary ip + port endpoint
```

Revision 0 only needs a centralized `native_endpoint` seam. Exact view/storage
types, mutability, and public visibility can be refined during implementation.

## Native Socket Ownership

UDP and TCP public types should not inherit from one generic public socket
abstraction. Their protocols have different semantics and operation sets.

They may share an internal move-only RAII owner for the native socket handle.
That owner should:

- Represent valid versus empty ownership.
- Close a valid native socket during destruction.
- Prevent copying and double close.
- Support ownership transfer into final public resource types.
- Preserve native errors close to failing operations.
- Hide POSIX descriptor versus future Windows socket-handle differences.

The shared handle is implementation reuse, not a claim that UDP and TCP expose
the same public behavior.

## Execution Strategy Template

The current public direction uses an execution-strategy template together with
an independent storage-policy template:

```cpp
udp_socket<strategy, storage>
tcp_connection_handler<strategy, storage>
tcp_connection<strategy, storage>
```

The execution strategy selects how completion is observed while the transport
type selects which operations exist and what those operations mean. Sync and
async sockets use unified operation names:

```cpp
udp_socket<sync, storage>.receive();
udp_socket<async, storage>.receive();

tcp_connection<sync, storage>.read();
tcp_connection<async, storage>.read();
```

Logically:

```text
sync
    operation completes before returning
    completion value is returned directly

async
    operation is initiated before returning
    future-like operation exposes eventual completion
```

The exact async operation type and implementation are deferred. Async TCP
connection establishment is a required future capability, but it does not need
to be designed before the first synchronous UDP vertical slice.

Connections accepted by a handler inherit its strategy:

```text
tcp_connection_handler<strategy, storage>
                |
              accept
                |
                v
tcp_connection<strategy, storage>
```

Normal callback usage can rely on generic lambdas so callers do not repeatedly
spell the strategy:

```cpp
handler.on_connection([](auto& connection) {
    connection.on_receive([](auto event) {
        process(event.bytes);
    });
});
```

CTAD does not automatically infer a class-template argument merely because a
template name is used as a lambda parameter type. Explicitly named callback
arguments would need the strategy or an alias. Generic lambdas are the current
beginner-facing answer.

Each execution strategy owns a nested runtime `params` type. Readiness polling,
thread ownership, handler scheduling, and cancellation remain intentionally
unresolved and must not be silently collapsed into the broad sync/async label.

## Storage Policies

Receive-byte ownership is an independent policy axis:

```cpp
udp_socket<strategy, storage>
tcp_connection<strategy, storage>
```

The revision 0 default is caller-owned storage. A successful receive transfers
an owning `std::vector<std::byte>` to the caller. Socket-owned borrowed storage
is optional future customization, not the beginner-facing default.

Possible policy families include:

```text
caller_owned_storage    owning bytes returned to the caller
internal_storage        socket-owned reusable bytes and borrowed views
pooled_storage          owning leases returned to a pool
allocator-aware storage later
```

Each storage policy defines and interprets its own nested runtime `params` type.
The same convention applies to execution strategies:

```cpp
struct sync {
    struct params;
};

struct caller_owned_storage {
    struct params;
};
```

Concepts may eventually enforce these policy shapes. They do not need to be
implemented before the first vertical slice.

Kernel socket receive-buffer capacity and application datagram storage are
different settings. A storage policy controls application-side bytes; native
socket options control the OS queue.

## UDP Resource Model

Revision 0 currently favors one public UDP resource type:

```cpp
udp_socket<strategy, storage>
```

Loose and explicitly bound UDP sockets have substantially the same operation
set. Both can send datagrams, both can receive, and a loose socket may become
implicitly bound by the OS. The difference is how the local endpoint is chosen,
not a clean separation of capabilities.

Public socket resources are created only through fallible protocol-facade
factories. Their constructors remain private so callers cannot create an
unchecked or invalid native resource.

The factory functions deduce policy types from ordinary function parameters:

```cpp
auto socket = udp::open();
auto bound = udp::bind(local_endpoint);

auto customized = udp::bind(
    local_endpoint,
    async{async::params{/* settings */}},
    caller_owned_storage{
        caller_owned_storage::params{/* settings */}
    }
);
```

The default result type is conceptually:

```cpp
std::expected<
    udp_socket<sync, caller_owned_storage>,
    udp_socket_error
>
```

`udp::open()` creates a valid loose UDP socket, normally for sender-style use.
`udp::bind(endpoint)` creates a valid socket bound to the supplied local
endpoint, normally for receiving. Both return the same resource type. A loose
socket's native address-family default is provisionally IPv4 with an explicit
IPv6 override to be shaped later.

Direct operations conceptually include:

```cpp
std::expected<std::size_t, udp_send_error>
socket.send_to(
    endpoint destination,
    std::span<const std::byte> payload
);

std::expected<udp_datagram, udp_receive_error>
socket.receive();
```

with a caller-owned transport-specific result:

```cpp
struct udp_datagram {
    std::vector<std::byte> bytes;
    endpoint sender;
    bool truncated;
};
```

Returning the successful send byte count intentionally preserves evidence for
application protocol code. A zero-length UDP datagram remains a successful
receive with an empty byte vector; UDP has no TCP-style EOF.

The preferred event-driven operation conceptually resembles:

```cpp
socket.on_receive([](udp_datagram datagram) -> void {
    process(std::move(datagram));
});
```

The callback must return exactly `void` for revision 0. It handles one successful
datagram and does not control loop continuation through a return value. The
`on_receive()` call has a strategy-dependent result:

```text
sync
    blocks, invokes callbacks sequentially on the calling thread,
    and directly returns terminal completion/error information

async
    returns a future-like operation whose eventual result contains
    equivalent terminal completion/error information
```

Fine-grained operation types and threading mechanisms are deferred. A connected
UDP type may eventually deserve a separate capability model because native UDP
connect changes peer filtering and permits destination-free send/receive calls.

## TCP Resource Model

The current TCP model uses two distinct public roles.

### `tcp_connection_handler<strategy, storage>`

This is the bound server-side resource. Its intended responsibilities are:

- Own the listening native socket.
- Accept incoming connections directly when requested.
- Provide a preferred event-driven connection callback loop.
- Potentially coordinate internally managed accepted connections.
- Report its local endpoint.

The name intentionally emphasizes that the resource handles incoming
connections rather than reading application bytes itself. Within dirtyNet
documentation, `connection_callback` should name the user callable so it is not
confused with the owning `tcp_connection_handler` resource.

Provisional creation and direct acceptance use the protocol facade:

```cpp
auto handler = tcp::bind(local_endpoint, strategy{}, storage{});

std::expected<tcp_connection<strategy, storage>, tcp_accept_error>
handler.accept();
```

The `bind` factory may perform native socket creation, option defaults, bind,
and listen as one high-level operation. This intentionally provides more hand
holding than the raw POSIX sequence. Listen backlog and configuration placement
remain open.

The event-driven shape is:

```cpp
handler.on_connection([](auto& connection) {
    connection.on_receive(read_callback);
});
```

Direct `accept()` and managed `on_connection()` should not consume connections
from the same native accept queue concurrently without an explicit policy.
Revision 0 needs a guard or mode boundary for this conflict.

### `tcp_connection<strategy, storage>`

This represents one established TCP byte-stream connection, whether created by
an outbound client connection or returned by server-side acceptance.

Responsibilities include:

- Own the connected native socket.
- Expose local and remote endpoints.
- Provide direct byte-oriented read and write operations.
- Provide preferred event-driven receive handling.
- Represent EOF, shutdown, cancellation, and transport errors distinctly.

Provisional outbound creation:

```cpp
auto connection = tcp::connect(remote_endpoint, strategy{}, storage{});
```

Provisional direct I/O:

```cpp
std::expected<std::size_t, tcp_read_error>
connection.read(mutable_byte_view destination);

std::expected<std::size_t, tcp_write_error>
connection.write(byte_view source);
```

These signatures are not final. TCP EOF, partial writes, nonblocking progress,
and cancellation may require richer result types.

Preferred event-driven use resembles:

```cpp
connection.on_receive([](tcp_read_event event) {
    process(event.bytes);
});
```

## Common I/O Byte Model

dirtyNet should define a common byte-storage vocabulary for socket I/O. The
same byte view/storage concepts can be used across transports without erasing
transport-specific event meaning.

The beginner-facing default uses owning bytes:

```cpp
struct udp_datagram {
    std::vector<std::byte> bytes;
    endpoint sender;
};

struct tcp_read_data {
    std::vector<std::byte> bytes;
};
```

UDP events represent complete datagrams unless truncation is explicitly
reported. TCP events represent available stream chunks and must never imply
application message boundaries. Advanced borrowed, pooled, or allocator-aware
representations can expose the same logical byte access through other storage
policies later.

## Event-Driven And Direct APIs

The public APIs should have a recognizable family resemblance:

```cpp
udp_socket.on_receive(datagram_callback);
tcp_connection.on_receive(stream_callback);
tcp_connection_handler.on_connection(connection_callback);
```

The event-driven machinery conceptually performs:

```text
wait for readiness or completion
              |
              v
perform transport-specific native operation
              |
              v
construct typed event or connection
              |
              v
invoke user callback according to strategy
              |
              v
continue until stop, cancellation, EOF, or error
```

Direct operations remain available:

```cpp
udp_socket.send_to(...);
udp_socket.receive_from(...);

tcp_connection_handler.accept();

tcp_connection.read(...);
tcp_connection.write(...);
```

The event API should be implemented from the same transport contracts, error
model, and native ownership rules. It should not become a behaviorally separate
socket system.

Event data callbacks return exactly `void`. Receive-loop completion and errors
belong to the strategy-dependent result of `on_receive()`, not to each
successful-data callback.

## Resource Termination

Socket-owning resources expose an intentionally abrasive termination operation:

```cpp
void kill() noexcept;
```

`kill()` is an unconditional, abortive termination boundary:

- It never throws.
- It is safe to request repeatedly.
- It prevents future I/O and callback registration.
- It requests termination of active operations.
- It releases the native resource on a best-effort basis.
- Pending operations complete with a killed terminal state rather than an
  exception.
- The killed object remains permanently unusable and cannot be reopened,
  rebound, reconnected, or restarted.

Creating another usable resource requires another protocol-factory call.
Graceful TCP shutdown is a separate later contract. Resource destructors use
the same non-throwing cleanup foundation.

## Error Model

Revision 0 construction factories use `std::expected`.

A stable dirtyNet error category is useful for portable control flow, but an
enum alone may discard essential diagnostic information. A likely direction is
a structured error containing both:

```cpp
struct socket_error {
    dirtynet_error_code code;
    native_error_code native_code;
};
```

Examples of stable categories may include socket creation, invalid state,
unsupported family, bind, listen, accept, connect, read, write, cancellation,
and peer closure. Exact type granularity and naming are unresolved.

Native error capture must happen close to the failed operation before unrelated
calls can overwrite it. POSIX `errno` and Windows socket errors should be
preserved without forcing normal application logic to depend on either.

## Explicitly Deferred Features

The following should not dictate the revision 0 core unless implementation
experiments demonstrate an immediate need:

- Windows backend implementation
- Thread-safe cached IP presentation strings
- Canonical/compressed IP string policy
- Connected UDP public type
- Multicast membership API
- DNS result collections, preference, and fallback policies
- TLS
- HTTP
- Application packet schemas
- Required serialization framework
- Built-in market-data protocol
- Generic public UDP/TCP base socket
- Production thread-pool tuning
- Multiple readiness backends
- Scatter/gather and zero-copy claims
- Throwing convenience constructors

## Revision 0 Distribution And Source Boundary

Revision 0 will begin as a primarily header-only C++ library. Strategy and
storage-policy template definitions naturally remain visible in public headers,
and a header-first implementation keeps early consumption and iteration simple.

Header-only does not mean that native details may spread throughout the public
API. POSIX-specific behavior should remain behind identifiable internal seams
such as:

```cpp
dirtynet::detail::native_socket
dirtynet::detail::native_endpoint
```

The first build target can be a CMake `INTERFACE` library. Native wrappers can
be defined inline for revision 0 and moved into compiled source files later
without changing the public responsibility model.

A provisional layout is:

```text
include/dirtynet/
|-- dirtynet.hpp
|-- endpoint.hpp
|-- ip.hpp
|-- port.hpp
|-- udp.hpp
|-- strategy/
|-- storage/
`-- detail/
    |-- native_endpoint.hpp
    |-- native_socket.hpp
    `-- posix/

tests/
|-- address/
|-- udp/
`-- integration/

sandbox/
`-- udp-pingpong/
    `-- readme.md
```

The exact file granularity remains open. Every sandbox subproject must retain a
short `readme.md` with strict exercise goals.

## Revision 0 Success Shape

The design phase is successful when it produces enough stable responsibility
boundaries to implement small POSIX vertical slices without inventing the API
inside each example.

A plausible implementation sequence, subject to later approval, is:

1. Address value types and native endpoint conversion.
2. Internal native socket-handle ownership.
3. Direct blocking UDP operations.
4. Direct blocking TCP connection handling and connected I/O.
5. Common byte views and transport-specific read results.
6. Synchronous callback loops built over direct operations.
7. Async strategy experiments informed by explicit lifetime and cancellation
   contracts.

No C++ implementation is authorized by this design document.

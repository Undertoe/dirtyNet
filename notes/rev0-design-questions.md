# dirtyNet Revision 0 Design Questions

## Purpose

This document is the detailed decision queue for continuing the dirtyNet
revision 0 design. It separates questions that shape the public contract from
implementation experiments that can wait.

Each section records:

- the decision that must eventually be made;
- why it matters;
- plausible options;
- consequences to inspect;
- a useful next discussion or experiment.

The order below is intentional. Ownership and behavior should be understood
before selecting epoll structures, thread counts, or advanced templates.

## Decisions Recorded On 2026-08-07

The following high-level decisions reduce the open space in later sections:

- Sync and async strategies use unified operation names; the strategy changes
  how completion is observed.
- Storage ownership is an independent policy axis.
- The default storage policy transfers caller-owned
  `std::vector<std::byte>` values.
- Strategy and storage policy types each own a nested runtime `params` type.
- Non-template `udp` and `tcp` protocol facades expose function-template
  factories that deduce strategy and storage policy objects from parameters.
- Public socket-resource constructors remain private; factories return
  `std::expected` and only expose valid resources.
- UDP `send_to()` returns the successful byte count.
- UDP `receive()` returns an owning datagram containing bytes and sender.
- UDP zero-byte datagrams are successful data, not EOF.
- `on_receive()` data callbacks return exactly `void`.
- The first synchronous UDP `on_receive()` handles one datagram and returns;
  caller code owns repetition explicitly, such as with a `while` loop.
- The preferred UDP ping/pong shape uses callback-based receive on the server
  and direct blocking receive on the one-request/one-response client.
- Packet-shaped outbound values may opt into socket send/write overloads by
  exposing their encoded bytes; byte-oriented I/O remains the foundation.
- The packet concept does not extend to typed receive/read operations; inbound
  protocol decoding remains above dirtyNet.
- Pass 0 includes basic numeric IPv4 and IPv6 value support.
- Hostname resolution is an explicit early follow-up capability rather than an
  IP constructor behavior. It returns multiple endpoint candidates for caller
  or connection-attempt policy to consider.
- Public address/resource values do not have an observable invalid state.
  Fallible or untrusted construction boundaries use expected-returning factory
  functions; already-valid address values may compose through ordinary
  constructors.
- An endpoint can be constructed directly from a valid IP value and port.
  Endpoints reported by socket operations are validated while converting from
  native address data, so the resulting public endpoint is valid.
- IP values store an already-parsed binary address through the native backend
  seam. An endpoint stores its semantic `ip + port` values and eagerly prepares
  the complete native endpoint representation used by socket calls.
- Socket-owning resources expose permanent, idempotent `kill() noexcept`
  semantics; killed resources cannot be reused.
- Revision 0 starts primarily header-only while isolating native backend details
  for later extraction into a compiled library.

## Priority 1: Define The Execution Strategy Contract

### Question

What does the template argument in these types promise?

```cpp
udp_socket<strategy, storage>
tcp_connection_handler<strategy, storage>
tcp_connection<strategy, storage>
```

### Why It Matters

The word `strategy` can accidentally combine several independent choices:

- whether the initiating call blocks;
- whether native sockets use blocking or nonblocking mode;
- how readiness is detected;
- which threads wait for I/O;
- which threads run user callbacks;
- whether callbacks for one connection can overlap;
- how cancellation is requested and observed;
- whether an external event loop or executor is required.

If the template parameter means all of these without a precise contract, types
will appear strongly specified while their actual behavior remains surprising.

### Candidate Models

#### A. Strategy Means Only `sync` Or `async`

```cpp
udp_socket<sync, caller_owned_storage>
udp_socket<async, caller_owned_storage>
```

The implementation selects strong defaults for polling and threading.

Benefits:

- Small beginner-facing type vocabulary.
- Easy discovery and explanation.
- Implementation choices can evolve behind the broad contract.

Pressures:

- `async` still needs a documented scheduling and ownership contract.
- Advanced users may eventually need executor or backend control.
- Changing callback-thread behavior later may be a breaking semantic change.

#### B. Strategy Is A Complete Execution Policy

```cpp
udp_socket<single_thread_epoll>
udp_socket<epoll_thread_pool>
udp_socket<blocking_caller_thread>
```

Benefits:

- Behavior is explicit in the type.
- Specialized implementations can be selected at compile time.

Pressures:

- Large public type vocabulary.
- Polling backend and callback scheduling become tightly coupled.
- Windows strategies will not map directly to POSIX names.
- Beginner-facing APIs become harder to read.

#### C. Strategy Selects Broad Semantics And Accepts Runtime Context

```cpp
udp_socket<async, caller_owned_storage> socket{io_context};
```

The type selects sync/async behavior while an injected context owns polling,
threads, or scheduling.

Benefits:

- Separates public semantic mode from runtime resources.
- Multiple sockets can share a context and thread pool.
- Backend-specific context internals can remain hidden.

Pressures:

- Reintroduces a context object and associated boilerplate.
- Context lifetime must dominate all registered operations.
- A strong default/global convenience path may still be needed.

### Questions To Answer Next

1. Should a later synchronous repeated-receive operation have a distinct name,
   or should `on_receive()` eventually grow that behavior?
2. Does asynchronous `on_receive()` return immediately?
3. Does dirtyNet create threads by default, or must callers provide a context?
4. May two callbacks for one socket or connection execute concurrently?
5. Must the strategy type appear in every accepted connection type?
6. Is backend selection compile-time, runtime, or entirely internal for rev0?

### Suggested First Discussion

Describe one synchronous UDP receive loop and one asynchronous UDP receive loop
from the caller's perspective. Do not discuss epoll yet. Record who blocks, who
owns the operation, which thread calls the handler, and how the loop stops.

## Priority 2: Define Event Operation Ownership

### Question

What is created and returned by an event-registration call?

```cpp
socket.on_receive(callback);
handler.on_connection(callback);
```

### Why It Matters

A repeated receive or accept loop has lifetime even when the socket object
remains alive. Users need a predictable way to stop, wait for, inspect, or
destroy that activity.

### Candidate Models

#### A. The Socket Owns One Implicit Operation

```cpp
std::expected<void, udp_event_error>
socket.on_receive(callback);

socket.stop_receive();
```

Benefits:

- Minimal return-type ceremony.
- Natural for one receive loop per socket.

Pressures:

- Socket accumulates lifecycle methods and state.
- Replacing handlers needs a policy.
- Destruction during callback execution becomes subtle.

#### B. Return An Explicit Operation Handle

```cpp
std::expected<receive_operation<strategy>, udp_event_error>
operation = socket.on_receive(callback);

operation->stop();
operation->wait();
```

Benefits:

- Cancellation and waiting have an obvious owner.
- Socket and operation lifetimes are distinguishable.
- Multiple kinds of activity can use a common lifecycle vocabulary.

Pressures:

- The operation must coordinate lifetime with the socket.
- Temporary operation handles can cause accidental immediate cancellation if
  destructor semantics are not obvious.
- Sync operations may make the handle feel artificial.

#### C. A Server/Socket `run()` Owns All Registered Handlers

```cpp
socket.on_receive(callback);
std::expected<void, run_error> result = socket.run();
```

Benefits:

- Registration and execution phases are explicit.
- Multiple callbacks can be configured before I/O starts.

Pressures:

- Adds a lifecycle state machine.
- Async `run()` still needs a return and shutdown contract.
- Can resemble the context boilerplate dirtyNet wants to simplify.

### Questions To Answer Next

1. Can more than one receive callback be registered per connection?
2. Does registering a callback start I/O immediately?
3. What does destroying the returned operation do?
4. Can an operation outlive the public socket object?
5. Is stopping synchronous listening possible from another thread?
6. Does stop wait for a running callback, or merely prevent the next callback?

## Priority 3: Define Accepted TCP Connection Ownership

### Question

Who owns a connection produced by event-driven acceptance?

```cpp
handler.on_connection([](auto& connection) {
    // Who keeps this alive after the callback returns?
});
```

### Direct Mode Baseline

Direct acceptance clearly transfers ownership:

```cpp
std::expected<tcp_connection<strategy, storage>, tcp_accept_error>
handler.accept();
```

The caller owns the returned move-only connection.

### Candidate Event Models

#### A. `tcp_connection_handler` Owns Accepted Connections

The callback receives a managed reference or handle. The handler removes a
connection after EOF, close, cancellation, or fatal error.

Benefits:

- Strong hand-holding and natural server behavior.
- Users can configure callbacks without building a connection container.
- Internal state can coordinate shutdown and connection counting.

Pressures:

- A safe reference/handle lifetime must be defined.
- Application code may need a way to retain a connection handle for targeted
  writes.
- Server destruction and per-connection shutdown need ordering rules.

#### B. The Connection Callback Receives Ownership

```cpp
handler.on_connection([](tcp_connection<strategy, storage> connection) {
    application_connections.add(std::move(connection));
});
```

Benefits:

- Clear unique ownership.
- Server acceptance machinery stays small.
- Applications control storage and connection lifetime.

Pressures:

- Easy for a beginner to let the connection die when the callback returns.
- Every event-driven server must build connection management.
- Shutdown coordination moves into application code.

#### C. Callback Configures A Connection Then Returns It To Management

The callback receives temporary configuration access, installs read/error
handlers, and dirtyNet retains the connection afterward.

Benefits:

- Encourages a clear setup phase.
- Preserves internal management.

Pressures:

- Returning or retaining access after setup needs another handle type.
- Failure or rejection during configuration needs a contract.

### Related Decisions

- How an application identifies a connection for later writes.
- Whether connections have stable IDs in addition to endpoints.
- Whether the same remote endpoint can identify multiple simultaneous TCP
  connections.
- Whether a handler can reject a connection before receive registration.
- Whether accepted connections inherit all strategy/context configuration.
- Whether direct `accept()` and `on_connection()` are mutually exclusive modes.

### Important Endpoint Note

Endpoints alone are not guaranteed unique connection identities. Multiple TCP
connections can involve the same remote IP and port at different times, and
local endpoint details also participate in a connection tuple. Connection
management should not assume an endpoint is a permanent unique key.

## Priority 4: Refine Byte And Event Ownership Beyond The Default

### Question

The default callback receives caller-owned bytes. What additional storage
policies, if any, must revision 0 implement?

```cpp
connection.on_receive([](tcp_read_event event) {
    use(event.bytes);
});
```

### Candidate Models Beyond Caller-Owned `std::vector<std::byte>`

#### A. Borrowed View Valid Only During Callback

Benefits:

- Minimal allocation and copying.
- Buffer reuse is straightforward.
- Good fit for parsers that consume immediately.

Pressures:

- Retaining the view is unsafe.
- Async application work must copy explicitly.
- The lifetime rule must be conspicuous and hard to misuse.

#### B. Callback Receives Owned Buffer

Benefits:

- Simple lifetime model.
- Handler can move work to another thread or retain data.

Pressures:

- Allocation and buffer-transfer costs.
- Pooling and reuse become implementation concerns.
- Backpressure is required if handlers retain buffers indefinitely.

#### C. Shared Or Reference-Counted Buffer

Benefits:

- Cheap handoff when retained.
- Internal pooling may coexist with asynchronous consumers.

Pressures:

- Atomic reference-count overhead.
- Pool lifetime and reclamation complexity.
- May be too sophisticated for revision 0.

#### D. Separate Borrowed And Owning APIs

Benefits:

- Fast path and safe-retention path are both explicit.

Pressures:

- More API surface and documentation.
- Callback overload selection may become confusing.

### Transport-Specific Requirements

UDP events need to preserve:

- one-datagram boundaries;
- sender endpoint;
- received size;
- truncation information when the buffer is insufficient.

TCP events need to preserve:

- received byte count;
- distinction between data, EOF, temporary no-progress, cancellation, and
  fatal error;
- the fact that a callback chunk is not an application message.

### Questions To Answer Next

1. Is immediate callback consumption the intended revision 0 happy path?
2. May user callbacks dispatch work onto other threads?
3. Can users supply receive buffers directly?
4. Does the library own a buffer per socket, per operation, or per event?
5. How is maximum UDP datagram size or truncation configured?
6. Does handler completion release buffer capacity and create backpressure?

## Priority 5: Define Error, EOF, And Callback Failure Delivery

### Question

How do failures that occur after successful factory construction reach event
users?

### Candidate Channels

#### A. Event Contains Data Or Error

```cpp
connection.on_receive([](std::expected<tcp_read_event, tcp_read_error> event) {
    // ...
});
```

Benefits:

- One ordered event stream.
- Every handler invocation explicitly acknowledges failure.

Pressures:

- Normal data-handling code becomes noisier.
- Terminal versus recoverable errors still need classification.

#### B. Separate Data And Error Callbacks

```cpp
connection.on_receive(data_callback);
connection.on_error(error_callback);
```

Benefits:

- Clean happy-path data callback.
- Error policy can be shared or defaulted.

Pressures:

- Ordering between callbacks must be guaranteed.

- Missing error callbacks need a safe default.
- Registration timing can create races if I/O starts immediately.

#### C. Operation Completes With A Terminal Result

```cpp
auto operation = connection.on_receive(data_callback);
std::expected<receive_summary, tcp_read_error> result = operation.wait();
```

Benefits:

- Data callback stays focused.
- Terminal outcome belongs to operation lifetime.

Pressures:

- Async users still need notification without blocking in `wait()`.
- Recoverable/transient events need internal policy.

### Required Distinctions

At minimum, the design must keep these concepts distinct:

- TCP peer EOF;
- local cancellation;
- timeout, if supported;
- recoverable readiness/no-progress condition;
- native transport failure;
- application callback failure or exception;
- malformed application data, which is normally above core dirtyNet.

### Callback Exceptions

Questions include:

- Are exceptions allowed to escape a synchronous callback loop?
- What catches an exception on a dirtyNet-owned async thread?
- Does one callback exception close only its connection or stop a whole server?
- Can the strategy install a callback-exception policy?

Revision 0 can initially require callbacks not to throw, but the behavior must
still be documented rather than left to thread termination.

## Priority 6: Define Direct TCP Read And Write Semantics

### Read

Questions:

- Does `read(buffer)` mean one native read attempt or fill the entire buffer?
- Is there a separate `read_exactly(buffer)` helper?
- How is TCP EOF represented without confusing it with an empty application
  message?
- For nonblocking strategies, how is would-block represented?
- Are cancellation and timeout parameters part of direct operations?

Potential separation:

```cpp
std::expected<tcp_read_result, tcp_read_error>
connection.read_some(mutable_byte_view destination);

std::expected<void, tcp_read_error>
connection.read_exactly(mutable_byte_view destination);
```

### Write

Questions:

- Does `write()` perform one native attempt and report a partial count?
- Does a beginner-facing `write_all()` loop until complete?
- How is partial progress preserved when an error occurs?
- Can several async writes be initiated concurrently?
- Does dirtyNet serialize writes per connection, or is that caller policy?
- Who owns an async write buffer until completion?

Potential separation:

```cpp
std::expected<tcp_write_result, tcp_write_error>
connection.write_some(byte_view source);

std::expected<std::size_t, tcp_write_error>
connection.write_all(byte_view source);
```

Exact names are open. The important point is not to hide TCP partial-I/O
behavior behind an ambiguous `read` or `write` contract.

## Priority 7: Define Direct And Event-Driven UDP Semantics

### Loose Versus Bound Creation

Current direction uses one `udp_socket<strategy, storage>` type returned from two
factories:

```cpp
auto socket = udp::open(strategy{}, storage{});

auto bound = udp::bind(endpoint, strategy{}, storage{});
```

Open questions:

- Which address family does parameterless `open()` use?
- Should `open()` require `ip_family` to avoid later destination mismatch?
- May `on_receive()` implicitly bind a loose socket to any-address and port 0?
- Does `local_endpoint()` force discovery of an OS-selected endpoint?
- Is explicit binding required for the preferred receive path?

### First Synchronous Callback Shape

For the first UDP slice, `on_receive(callback)` is a one-shot blocking
operation. It receives one datagram, invokes the callback exactly once on the
calling thread, and then returns completion or error information. The
application owns repetition explicitly:

```cpp
while (running) {
    auto result = server.on_receive([&server](auto datagram) {
        server.send_to(datagram.sender, pong);
    });

    if (!result) {
        break;
    }
}
```

The one-request/one-response client does not need a callback. It sends directly
and then calls the direct blocking UDP receive operation once.

This deliberately postpones stop tokens, operation handles, and cross-thread
cancellation. A later repeated-receive convenience may be added after those
lifetime contracts are designed.

### Receive Results

Questions:

- How is datagram truncation reported?
- Does a zero-byte UDP datagram remain a successful event?
- Is the sender endpoint always materialized?
- Can callers request destination/interface metadata later?
- Does the event-driven API allocate a maximum-size buffer or use configured
  capacity?

### Connected UDP

Connected UDP changes semantics enough that it may eventually justify:

```cpp
connected_udp_socket<strategy, storage>
```

This should remain deferred until raw POSIX experiments show which capabilities
and errors should be exposed.

## Priority 8: Refine Factory Naming And Parameter Ordering

### Current Direction

Protocol-facade function templates deduce policy arguments:

```cpp
auto socket = udp::bind(endpoint, strategy{}, storage{});
auto handler = tcp::bind(endpoint, strategy{}, storage{});
auto connection = tcp::connect(endpoint, strategy{}, storage{});
```

The default arguments select `sync` and `caller_owned_storage`. No intermediate
`socket_config` object is currently wanted.

### Evaluation Criteria

- Does the call clearly communicate the result type?
- Can a beginner discover all normal protocol operations from one entry point?
- Does the type name describe a resource or merely act as a namespace?
- How much template syntax appears in the happy path?
- Can factories share configuration without introducing a context prematurely?
- Does UDP naming provide evidence for TCP naming rather than forcing symmetry?

Complete usage examples should still verify argument ordering and whether the
`udp`/`tcp` facade names remain readable in practice.

## Priority 9: Define Hostname Resolution Boundaries

Explicit `ipv4` and `ipv6` construction parses numeric addresses. Hostnames
such as `feed.example.com` use a separate, fallible resolution operation.

Resolution is required early for the intended market-data proof of concept,
but it does not need to be part of the first basic address/UDP implementation
pass. A successful resolution returns all usable endpoint candidates rather
than silently selecting the OS resolver's first result. Candidate ordering may
still follow the resolver, while connection-attempt and fallback policy belongs
to the caller, a transport connection operation, or a later helper.

Questions:

- What should the resolver facade and result collection be named?
- Should a distinct `hostname` type prevent confusion with numeric literals?
- How is blocking DNS behavior communicated when used with an async strategy?
- Does connection fallback belong to resolution, TCP connect, or a higher layer?
- How are native resolver errors represented?

Potential expected-based shape:

```cpp
std::expected<std::vector<endpoint>, resolve_error>
resolve(std::string_view hostname, port service_port);
```

The exact facade and collection type remain open. The important boundary is
that resolution is explicit, fallible, and potentially one-to-many.

## Priority 10: Define Endpoint Native Access

Revision 0 needs a replaceable POSIX seam, not a perfected cross-platform
framework.

Questions:

- Is `endpoint::native()` public interoperability API or backend-only access?
- Does it return a view, pointer-length pair, or named native wrapper?
- Does `from_native` validate unsupported families and lengths through
  `std::expected`?
- Are endpoint objects immutable after construction?

Current representation decision:

```text
endpoint
|-- ip
|   `-- already-parsed binary address
|-- port
`-- eagerly prepared native endpoint storage + length
```

The concrete binary/native types may be POSIX `in_addr`, `in6_addr`, and
socket-address structures in the first backend, with Windows equivalents later.
Those types do not become the platform-neutral public identity of an address or
endpoint.

Performance intent:

- Repeated UDP send-to operations should not allocate, parse, resolve, or
  repeatedly reconstruct an unchanged destination.
- Connected TCP read/write operations normally use only the connection handle;
  endpoints are primarily setup and reporting values there.

## Priority 11: Define Error Type Granularity

Detailed error taxonomy is deferred until after the first address and native
endpoint implementation pass. The first pass may use a small provisional result
shape so implementation evidence can guide later distinctions. Those initial
error names and categories are not considered stable public API commitments.

Questions:

- One `socket_error` type with operation/category fields, or separate error
  types for bind/connect/read/write/accept?
- Enum-only errors or structured errors preserving native codes?
- Should native error access be portable through `std::error_code`?
- Which errors are suitable for application control flow?
- How are error messages formatted without allocating on hot paths?
- Are error values trivially copyable/destructible where possible?

A useful rule is that dirtyNet categories support portable decisions while
native codes preserve diagnostic evidence.

## Priority 12: Configuration And Defaults

The library should provide strong defaults but eventually allow explicit
control.

Potential configuration includes:

- TCP listen backlog;
- address and port reuse;
- IPv6 dual-stack behavior;
- blocking/nonblocking native mode;
- send and receive buffer sizing;
- no-delay behavior;
- keepalive;
- multicast interface and membership;
- timeouts;
- executor/context selection;
- callback concurrency policy.

Questions:

- Which options must be set before native socket creation?
- Which belong to factory parameters versus an options value?
- Which defaults must be identical across OSes versus merely behaviorally
  sensible?
- Are options immutable values reusable across several factories?
- Do protocol-specific option types provide better guard rails than one generic
  socket-options bag?

Revision 0 should expose only options needed by its vertical slices, while
reserving a coherent placement for later additions.

## Priority 13: Thread Safety And Object Copy/Move Contracts

Likely baseline:

- Socket-owning resources are move-only.
- Address and endpoint values are copyable value types.
- Direct simultaneous operations are not implicitly safe unless documented.
- Event strategy determines callback concurrency.

Questions:

- May a connection be written from a thread other than its receive callback?
- Are multiple concurrent writes serialized or rejected?
- Can endpoint/string formatting be called concurrently?
- What happens when a connection is moved while operations are outstanding?
- Does starting an async operation pin internal shared state?
- Which public objects are safe to destroy from inside their own callback?

The deferred `ip_string` cache should be revisited only after general library
thread-safety rules exist.

## Priority 14: Handler Type And Storage

Conceptually useful callback categories include:

- `udp_read_callback` receiving a UDP event;
- `tcp_read_callback` receiving a TCP stream event;
- `connection_callback` receiving managed access to a TCP connection;
- error and closure callbacks, depending on the selected error channel.

Questions:

- Are handlers accepted as unconstrained templates?
- Are they stored through `std::function`, move-only function wrappers, or a
  custom type-erasure facility?
- Must move-only lambdas be supported?
- Is handler allocation acceptable during registration?
- Are callback return values used for continue/stop/reject decisions?
- How are handler signatures diagnosed through concepts?

Avoid fixing a concrete `std::function` alias until lifetime and copyability
requirements are known.

## Priority 15: Backpressure And Work Scheduling

An event-driven networking API can receive data faster than application
handlers consume it.

Questions:

- Does the I/O loop wait until a handler returns before reading again?
- Can events queue while handlers run?
- Are queue bounds configurable?
- What happens when a queue is full?
- Is one connection allowed to monopolize a worker?
- Are connection callbacks serialized relative to shutdown callbacks?
- If epoll and a worker pool are combined, who owns buffer data during handoff?

Revision 0 synchronous callback loops can provide the simplest natural
backpressure: do not read the next event until the current callback returns.
Async design must make its policy explicit.

## Priority 16: Shutdown And Cancellation

Questions:

- Is close graceful, abortive, or selectable?
- Can TCP write shutdown occur independently of read shutdown?
- Does stopping a server close active connections or only stop acceptance?
- Can a handler request its own connection's closure safely?
- Does cancellation generate an error callback, completion result, or silent
  terminal state?
- How does a synchronous blocking call wake when another thread requests stop?
- How are native handle reuse races prevented after cancellation?

Cancellation should be treated as a feature contract rather than whatever
error the native syscall happens to produce.

## Priority 17: Revision 0 Scope Gate

Before implementation, explicitly approve a minimal capability table.

The approved first implementation pass is intentionally narrower than the full
candidate revision 0 table: basic IPv4 and IPv6 values, `ip`, `port`, endpoint
composition, and the POSIX native endpoint seam. Blocking UDP follows after
that foundation. Detailed portable error taxonomy may be revised after this
first implementation evidence exists.

Candidate baseline:

| Area | Candidate revision 0 capability |
|---|---|
| Address | port, IPv4, IPv6, IP variant, endpoint |
| Native boundary | POSIX endpoint conversion and RAII socket handle |
| UDP direct | open, bind, send-to, receive-from |
| UDP callback | one-shot blocking receive callback; caller owns repetition |
| TCP server direct | bind/listen factory and accept |
| TCP client direct | connect factory |
| TCP connection direct | read-some and write-some/all contract |
| TCP events | connection callback and repeated read callback |
| Execution | blocking/synchronous baseline before async backend |
| Errors | expected-based factories and structured native-aware failures |

Items to explicitly approve or defer:

- asynchronous strategy in revision 0 versus revision 0.x;
- IPv6 implementation completeness;
- hostname resolution;
- multicast;
- connected UDP;
- timeouts;
- graceful TCP half-close;
- operation handles;
- thread pool;
- epoll.

## Recommended Next Conversation Order

1. Review and finish the documentation-only design branch.
2. Begin the approved address-foundation and POSIX native-endpoint pass using
   `notes/working-todos.md`.
3. Revisit portable error values with evidence from that implementation.
4. Approve and implement blocking caller-owned UDP bind/send/receive.
5. Validate the direct and one-shot callback paths with UDP ping/pong.
6. Defer detailed async operation handles, polling, and thread mechanics until
   blocking UDP behavior supplies implementation evidence.

## Questions That Do Not Need Answers Yet

These are worth retaining but should not block the current design:

- exact `sockaddr_storage` wrapper layout;
- exact atomic or mutex design for future IP string caching;
- canonical IPv6 text compression rules;
- exact epoll registration data structure;
- production worker-count defaults;
- IOCP mapping;
- TLS layering;
- packet/framing helper concepts;
- benchmark design;
- ABI stability;
- custom allocators;
- coroutine/awaitable syntax;
- C++26-only facilities.

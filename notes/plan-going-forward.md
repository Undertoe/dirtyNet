# Plan Going Forward

## Current Baseline

dirtyNet now has the first usable IPv4 UDP vertical slice:

- public port, IPv4/IP, and endpoint values;
- a POSIX native endpoint boundary;
- move-only UDP socket ownership;
- blocking bind, send, and receive operations;
- a dirtyNet UDP ping/pong proof;
- a header-only CMake `INTERFACE` library target.

The archived implementation remains a read-only reference. New work stays in
the current `lib/dirtynet/`, `tests/`, `sandbox/`, and performance structure.

## Project Direction

dirtyNet is a generic, composable network library. Its job is to remove raw
POSIX socket-management ceremony while retaining direct, efficient access to
network I/O. Demonstration applications provide demanding feedback, but their
protocol and data-model concerns remain above the core library.

The long-term library may provide optional execution, storage, batching,
cancellation, and timer strategies. These should grow from measured use cases
rather than being fixed before the blocking primitives are proven.

Direct byte-oriented operations remain the source of truth. Higher-level typed
or generic operations should compose over them without forcing a particular
wire protocol or application data model into dirtyNet.

## Near-Term Milestones

### 1. IPv4 Multicast Receive

- Add explicit static multicast binding factories to `udp_socket`.
- Support OS-selected and explicitly selected local IPv4 interfaces.
- Hide `SO_REUSEADDR`, wildcard binding, and `IP_ADD_MEMBERSHIP` behind the
  POSIX implementation boundary.
- Reuse the existing `send_to` and `receive` operations after construction.
- Prove local fanout in `sandbox/udp-many/`.

Do not introduce a public network-interface type solely for this first pass.
An IPv4 local-interface address is sufficient until another backend or use case
demands richer interface identity.

### 2. Continuous Integration

- Add GitHub Actions coverage for configure, build, and tests.
- Keep the first matrix small and representative of the project's supported
  environment.
- Include one AddressSanitizer and UndefinedBehaviorSanitizer configuration
  after the public UDP contract tests are reliable.
- Include the multicast behavior in automated testing where the runner permits
  reliable local multicast; otherwise retain a deterministic lower-level test
  and make the sandbox an explicit manual proof.

### 3. Performance Baseline

- Reconcile the current performance directory and CMake benchmark wiring.
- Define repeatable workloads, build mode, machine context, warmup, sample
  count, and reported statistics.
- Establish UDP receive throughput as the first baseline.
- Record datagrams/second and bytes/second, then add allocation, copy, CPU, and
  loss observations where useful.
- Compare changes against an identified baseline and investigate regressions.

### 4. External CMake Consumption

- Give consumers a stable namespaced target such as `dirtyNet::dirtynet`.
- Support use from a separate CMake project without copying source into that
  project.
- Start with `add_subdirectory` or `FetchContent`; add install/package-export
  support when an external consumer needs it.
- Validate the chosen workflow with a tiny out-of-tree consumer.

### 5. Minimal Blocking TCP

- Add a move-only outbound `tcp_connection` resource.
- Connect to an endpoint.
- Provide blocking `read_some` into caller-owned storage and `write_some` from
  caller-owned storage.
- Return byte counts and distinguish orderly EOF from errors.
- Support close and the minimal useful shutdown behavior.
- Keep application framing above the library; convenience `read_exactly` and
  `write_all` operations may compose over the primitives later.

The first TCP milestone does not include listeners, servers, async execution,
thread pools, callbacks, TLS, or an application protocol.

### 6. Hostname Resolution

- Keep hostname resolution separate from numeric IP parsing.
- Return multiple endpoint candidates so the caller can attempt connections in
  an intentional order.
- Preserve useful native diagnostics while exposing an OS-agnostic public
  result.

## Iteration Pattern

Each milestone must be divided into digestible API-centered work slices. For
each slice:

1. capture the concrete caller demand and the behavior it requires;
2. decide whether the current API already supports it;
3. if not, design the smallest reusable API update;
4. implement only the native behavior needed by that update;
5. test the public contract and relevant failure paths;
6. prove it in a focused sandbox or external consumer;
7. measure relevant costs;
8. record follow-up demands separately instead of growing the current slice.

API review is therefore part of every capability, not a cleanup phase after
implementation. Public naming, ownership, inputs, outputs, error behavior, and
composition with existing types should be settled for the current slice before
moving on. Provisional details may remain provisional when the use case has not
produced enough evidence.

Correctness, composability, and portability work may be performance-neutral,
but it should not silently reduce speed. Any meaningful regression needs to be
measured, explained, and deliberately accepted or corrected.

## Explicitly Later

- IPv6 completion and IPv6 multicast;
- multiple multicast memberships and explicit leave operations;
- source-specific multicast;
- multicast sender controls such as TTL, interface, and loopback;
- TCP listeners and accepted connections;
- library-owned repeated read loops and multi-socket execution strategies;
- caller-selectable pooled or allocator-aware storage;
- cancellation, timers, and retry policy;
- Windows backend work;
- TLS and application protocol layers.

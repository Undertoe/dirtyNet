# Working TODOs

This is the implementation checklist for the next dirtyNet milestones. It
records required behavior without making provisional signatures or result
types permanent.

## How To Use This Checklist

Do not implement an entire milestone as one change. Pull one coherent vertical
slice at a time through these checkpoints:

- [ ] State the caller demand and why the current API cannot satisfy it.
- [ ] Agree on the smallest public API update, including ownership and errors.
- [ ] Implement its native behavior without unrelated cleanup.
- [ ] Add focused public-contract and failure-path tests.
- [ ] Exercise it in the relevant sandbox or external consumer.
- [ ] Measure it when it affects a performance-sensitive path.
- [ ] Record newly discovered conveniences or extensions as later work.

An implementation exercise may show that no API change is required. Prefer
that result when the existing primitives compose cleanly.

## Implemented Baseline

- [x] Host-order `port` value and native conversion.
- [x] Numeric IPv4 parsing and public IP value types.
- [x] Public endpoint values and POSIX native conversion.
- [x] Move-only UDP socket ownership.
- [x] Blocking UDP bind, `send_to`, and receive operations.
- [x] Sender endpoint reporting.
- [x] A public truncation field exists on received datagrams.
- [x] dirtyNet UDP ping/pong proof.
- [x] Header-only CMake `INTERFACE` target and `dirtyNet::dirtynet` alias.

These items are the working foundation. Fix concrete defects as they appear,
but do not make broad UDP cleanup a prerequisite for multicast.

## Immediate Testing Debt

These are the largest gaps in the current automated suite. Address them in
small slices alongside multicast rather than turning them into an open-ended
UDP rewrite.

### Make The Existing Suite Reliable

- [ ] Replace the fixed UDP test port `1337` with an OS-selected ephemeral
  port. The fixed-port test currently fails when that port is already in use.
- [ ] Ensure every network test has a bounded completion path so a missing
  datagram fails instead of hanging the test process.
- [ ] Run tests from a fresh build tree in CI; do not depend on a build cache
  created at a different filesystem path or on another host.

### Cover The Public UDP Contract

- [ ] Add a loopback test through public `udp_socket` APIs that binds, sends,
  receives, and verifies the exact payload and sender endpoint.
- [ ] Test public `open()` and `bind()` success independently of the native
  implementation tests.
- [ ] Verify that a socket remains usable after move construction and move
  assignment, without duplicate descriptor ownership.
- [ ] Exercise representative open, bind, send, and receive failure paths and
  confirm failed construction does not leak a descriptor.
- [ ] Define and test the truncation contract. The public field exists, but the
  POSIX receive path currently always reports `false`.

### Add Multicast Contract Coverage

- [ ] Reject a non-multicast group endpoint with a predictable public error.
- [ ] Bind and join using the OS-selected interface path.
- [ ] Bind and join using an explicit loopback IPv4 interface.
- [ ] Bind two receivers to the same group and port, send one identified
  datagram, and verify that both receive it. This is the key proof that reuse,
  wildcard binding, and group membership compose correctly.
- [ ] Verify payload and sender endpoint preservation on multicast receive.
- [ ] Move a joined socket and verify that the moved-to socket still receives;
  membership and descriptor ownership must move together.
- [ ] Exercise failures after descriptor creation, especially bind and group
  join failures, to check cleanup on partial construction.

Use polling or a test-only bounded wait for delivery-sensitive integration
tests. Do not add timeout policy to the public UDP API merely to support the
test harness.

### Add Tooling Coverage After The Contract Tests

- [ ] Run the network tests under AddressSanitizer and UndefinedBehaviorSanitizer
  in at least one CI configuration.
- [ ] Keep local multicast delivery as a separate integration label if it is
  unreliable on a hosted runner; deterministic validation must still cover
  address validation and failure cleanup.

## Current Milestone: IPv4 Multicast

Implement this milestone as three reviewable slices:

1. settle the two public `bind_multicast` factory contracts and error behavior;
2. implement and test the POSIX construction path;
3. prove fanout through `sandbox/udp-many/` and record follow-up demands.

### Public Behavior

- [ ] Add a static `udp_socket::bind_multicast(group_endpoint)` factory.
- [ ] Add an overload accepting the local IPv4 interface address.
- [ ] Return the ordinary movable `udp_socket`; keep send and receive behavior
  unchanged after construction.
- [ ] Report invalid group, bind, and membership failures through the current
  error approach with useful native evidence.

### POSIX Behavior

- [ ] Create an IPv4 UDP descriptor.
- [ ] Enable the address reuse required for local multicast receivers.
- [ ] Bind `0.0.0.0` at the multicast group's port.
- [ ] Join the group with `IP_ADD_MEMBERSHIP`.
- [ ] Use the default interface when none is provided.
- [ ] Use the supplied local IPv4 address when an interface is provided.
- [ ] Ensure all failure paths close the descriptor.
- [ ] Confirm descriptor ownership and membership survive socket moves.

### Proof And Tests

- [ ] Complete the dirtyNet side of `sandbox/udp-many/` as a local multicast
  fanout proof.
- [ ] Complete the multicast contract coverage listed under immediate testing
  debt.

### Deferred From The First Multicast Pass

- explicit leave operations;
- multiple memberships per socket;
- IPv6 and source-specific multicast;
- multicast sender TTL, interface, and loopback controls;
- a public network-interface abstraction;
- generic socket-option exposure.

## Next Checkpoint: CI

- [ ] Add a GitHub Actions workflow for configure, build, and tests.
- [ ] Select a small supported compiler/build matrix.
- [ ] Avoid network- or timing-sensitive multicast tests as mandatory CI gates
  unless they are reliable on hosted runners.
- [ ] Keep the local multicast sandbox documented as a quick manual proof.

## Next Checkpoint: Performance Measurement

- [ ] Reconcile `perf/` with the current `benchmarks/` CMake wiring and choose
  one maintained location.
- [ ] Define release-build, warmup, repetition, and machine-context rules.
- [ ] Add a repeatable UDP receive workload with controlled payload sizes and
  message counts.
- [ ] Report datagrams/second and bytes/second as the primary throughput
  measures.
- [ ] Track allocations, copies, CPU use, loss, truncation, and sequence gaps
  when relevant to a change.
- [ ] Preserve historical results or commands well enough to compare meaningful
  iterations.
- [ ] Treat unexplained throughput degradation as a defect; do not use noisy
  microbenchmark thresholds as the first CI gate.

## External CMake Consumer

- [ ] Create a minimal separate-project proof that links
  `dirtyNet::dirtynet`.
- [ ] Confirm public include paths and compile requirements propagate from the
  target.
- [ ] Choose and document the initial consumption method.
- [ ] Add install/export packaging only when the separate consumer benefits
  from it.

## Minimal Blocking TCP

Begin with the outbound connection and partial byte-I/O contract. Keep
convenience loops, framing, listeners, and execution strategies in separate
follow-up slices.

- [ ] Add move-only native TCP connection ownership.
- [ ] Connect outbound to an endpoint.
- [ ] Add blocking `read_some` into caller-owned writable bytes.
- [ ] Add blocking `write_some` from caller-owned bytes.
- [ ] Return byte counts and distinguish EOF from an error.
- [ ] Add close and minimal shutdown behavior.
- [ ] Test partial reads/writes and peer closure.
- [ ] Prove the path with a small client exercise.

Do not require a TCP listener, server abstraction, threading model, callback
model, or TLS for this milestone.

## Hostname Resolution

- [ ] Resolve a host and service/port separately from numeric IP parsing.
- [ ] Return multiple endpoint candidates.
- [ ] Permit address-family filtering where useful.
- [ ] Preserve actionable native failure details.
- [ ] Prove connection attempts across returned candidates.

## Later Library Work

- IPv6 across address, endpoint, UDP, and TCP paths.
- `read_exactly` and `write_all` convenience algorithms.
- Optional typed serialization over the byte APIs.
- Repeated read and multi-socket execution strategies.
- Batching, cancellation, and timers.
- Caller-selectable storage and allocation strategies.
- TCP listening and accepted-connection ownership.
- Portable error-model refinement using evidence from UDP and TCP.
- Windows backend implementation.

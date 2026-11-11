# Work Index

Use this file to quickly answer: "What is the next work we should look at?"

## Quick Answer

Current work: add IPv4 multicast receive support to `udp_socket` on the
`feature/multicast` branch.

The existing IPv4 address, endpoint, move-only UDP socket, blocking
`send_to`/`receive`, and UDP ping/pong path are the working baseline. Do not
reopen those designs unless the multicast work exposes a concrete problem.

Primary implementation checklist: `notes/working-todos.md`.

Current direction and milestone order: `notes/plan-going-forward.md`.

Approved revision 0 background: `notes/rev0-design.md` and
`notes/rev0-design-questions.md`.

Continue to require Terry's explicit request before editing specific C++
sources, headers, benchmarks, or tests.

## Current Work Stream

### IPv4 Multicast

Goal: let a caller create an ordinary movable `udp_socket` that is already
bound to a multicast port and joined to one IPv4 multicast group.

The intended public entry points are static factories with these conceptual
forms:

- `udp_socket::bind_multicast(group_endpoint)` lets the OS choose the local
  interface.
- `udp_socket::bind_multicast(group_endpoint, local_interface_ip)` selects a
  local IPv4 interface explicitly.

The POSIX path should create the socket, enable the required address reuse,
bind the wildcard address at the group's port, join the group, and return the
same `udp_socket` type used by ordinary UDP operations. Membership follows the
native descriptor when the socket moves and ends when it closes.

Prove the behavior through the existing `sandbox/udp-many/` exercise. Keep the
first pass to IPv4 any-source multicast and one membership per socket.

## Ordered Task Queue

1. Implement the IPv4 multicast factory and dirtyNet fanout proof.
2. Establish GitHub Actions CI for supported configure, build, and test paths.
3. Establish a repeatable UDP performance baseline and reporting rules.
4. Make dirtyNet consumable from a separate CMake project.
5. Add a minimal move-only blocking outbound TCP connection.
6. Add hostname resolution that returns endpoint candidates.
7. Expand to the Bitcoin project.

The application project should feed requirements back into dirtyNet one small,
measured feature at a time. It does not redefine dirtyNet as an
application-specific networking library.

## Work-Slice Rule

Treat each numbered milestone as a direction, not as one oversized change. The
normal implementation slice is:

1. record one concrete usage demand;
2. decide the smallest public API change that satisfies it;
3. implement the corresponding native behavior;
4. add focused contract tests;
5. prove the API in a sandbox or external consumer;
6. measure it when the change can affect performance.

Finish and review that slice before absorbing adjacent conveniences. A demand
found in a demonstration project is evidence to consider an API update, not an
automatic requirement to add one.

## Measurement Rule

Performance work must be compared against a reproducible baseline. Changes
should not degrade throughput without an explicit, documented reason and
evidence. Improvements should report both rate and relevant costs rather than
only a single best-case number.

Useful initial observations include:

- datagrams and bytes processed per second;
- latency distribution where the workload makes latency meaningful;
- allocations and copies on the receive path;
- loss, truncation, and sequence gaps under load;
- CPU use and benchmark configuration.

CI should validate correctness. Performance measurements should initially be
reported rather than enforced as a noisy pass/fail gate.

## Later Design Questions

- What execution strategies should the library own for repeated reads and
  multiple sockets?
- Which responsibilities belong to a strategy, executor, or runtime context?
- Which caller-owned, pooled, or allocator-aware storage models earn a public
  abstraction?
- How should cancellation, timers, and repeated operation results compose?
- When should IPv6 multicast, multiple memberships, and source-specific
  multicast enter the API?
- What portable error detail is needed once UDP and TCP have both exercised the
  native boundary?

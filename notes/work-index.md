# Work Index

Use this file to quickly answer: "What is the next work we should look at?"

## Quick Answer

Next work: complete Step 0, the smallest IPv4-only dirtyNet UDP vertical slice:
`port` and `ipv4`, `ipv4` into `ip`, `ip + port` into `endpoint`, the POSIX
native endpoint seam, a baseline blocking UDP socket, and dirtyNet UDP
ping/pong.

Primary design reference: `notes/rev0-design.md`.

Detailed decision queue: `notes/rev0-design-questions.md`.

Implementation is active on `project-setup-ip`. Continue to require Terry's
explicit request before editing specific C++ sources, headers, benchmarks, or
tests.

## Current Work Stream

Step 0 proof-of-concept implementation.

Goal: prove the smallest end-to-end dirtyNet path with IPv4 UDP ping/pong, then
use that working path to choose socket-I/O defaults and options for the MVP.

Current implementation branch: `project-setup-ip`.

The revision 0 design is approved and merged. Baseline Linux socket behavior
and the existing UDP sandbox remain the implementation feedback loop.

## Prior Implementation Learning Stream

Baseline Linux socket behavior.

Goal: build understanding of raw Linux networking behavior before shaping dirtyNet abstractions.

Primary reference: `notes/plan-going-forward.md`.

Main library notes: `notes/dirtynet-library-notes.md`.

Planned feature notes: `notes/planned-features.md`.

Current UDP learning notes: `notes/udp-pingpong-library-notes.md`.

## Ordered Task Queue

1. Use `notes/working-todos.md` as the high-level implementation checklist.
2. Use the approved header-only `lib/dirtynet/*.hh` layout and CMake
   `INTERFACE` target shape when implementation begins.
3. Complete the IPv4-only Step 0 vertical slice described in
   `notes/working-todos.md`.
4. Validate Step 0 with dirtyNet UDP ping/pong.
5. Use Step 0 evidence to define baseline socket-I/O defaults and explicit
   options.
6. Complete IPv6 through the established address, endpoint, and UDP seams.
7. Revisit portable error/result granularity with implementation evidence.
8. Add the remaining capabilities required for the exchange-facing MVP.
9. Return to callback and async mechanics after the blocking contracts are
   proven.

## Follow-On Questions

- Which responsibilities belong to `strategy`, and which belong to a later
  runtime context or executor?
- Who owns event operations and accepted TCP connections?
- Which non-default borrowed, pooled, or allocator-aware storage policies are
  worth implementing after caller-owned storage is proven?
- How do repeated callback loops stop and report their terminal result?
- Should factories live on protocol facades or the resource types they return?
- Which baseline examples should live under `exec/`, `sandbox/`, or another intentionally chosen area?
- Should each baseline be a single executable with modes, or separate client/server executables?
- What behavior should be documented for each baseline before turning it into a reusable dirtyNet abstraction?
- Which pieces of the UDP ping-pong boilerplate should become the first `endpoint` and `udp_socket` APIs?

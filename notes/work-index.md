# Work Index

Use this file to quickly answer: "What is the next work we should look at?"

## Quick Answer

Next work: continue the revision 0 API design by defining the execution-strategy,
event-operation ownership, accepted-connection ownership, and byte-buffer
lifetime contracts.

Primary design reference: `notes/rev0-design.md`.

Detailed decision queue: `notes/rev0-design-questions.md`.

This work stream is documentation-only. Do not edit C++ source, headers,
benchmarks, or tests until Terry explicitly asks for the specific C++ work.

## Current Work Stream

Revision 0 system and public API design.

Goal: establish responsibility, ownership, event, error, and portability
contracts before implementation begins.

Current design branch: `rev0_design`.

After the design scope is approved, return to baseline Linux socket behavior as
the implementation feedback loop.

## Prior Implementation Learning Stream

Baseline Linux socket behavior.

Goal: build understanding of raw Linux networking behavior before shaping dirtyNet abstractions.

Primary reference: `notes/plan-going-forward.md`.

Main library notes: `notes/dirtynet-library-notes.md`.

Planned feature notes: `notes/planned-features.md`.

Current UDP learning notes: `notes/udp-pingpong-library-notes.md`.

## Ordered Task Queue

1. Walk through synchronous UDP callback behavior from socket creation through
   cancellation and destruction.
2. Define the equivalent asynchronous UDP behavior without selecting epoll or
   another native polling mechanism yet.
3. Define exactly what the socket execution-strategy template controls.
4. Define callback byte/event ownership and lifetime using UDP and TCP examples.
5. Define ownership of TCP connections produced by event-driven acceptance.
6. Define event-operation ownership, cancellation, waiting, and shutdown.
7. Define runtime error, EOF, cancellation, and callback-failure delivery.
8. Define direct TCP read/write partial-progress behavior.
9. Define the remaining direct and event-driven UDP receive behavior.
10. Compare complete UDP and TCP examples to settle factory placement and names.
11. Approve an explicit revision 0 capability table.
12. Decide the baseline executable layout and naming convention.
13. Resume POSIX UDP and TCP implementation experiments after explicit approval.

## Follow-On Questions

- Which responsibilities belong to `strategy`, and which belong to a later
  runtime context or executor?
- Who owns event operations and accepted TCP connections?
- Are callback bytes borrowed, owned, or available through separate APIs?
- How do repeated callback loops stop and report their terminal result?
- Should factories live on protocol facades or the resource types they return?
- Which baseline examples should live under `exec/`, `sandbox/`, or another intentionally chosen area?
- Should each baseline be a single executable with modes, or separate client/server executables?
- What behavior should be documented for each baseline before turning it into a reusable dirtyNet abstraction?
- Which pieces of the UDP ping-pong boilerplate should become the first `endpoint` and `udp_socket` APIs?

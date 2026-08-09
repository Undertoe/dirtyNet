# Work Index

Use this file to quickly answer: "What is the next work we should look at?"

## Quick Answer

Next work: begin the approved address-foundation implementation pass with basic
IPv4 and IPv6 values, `ip`, `port`, endpoint composition, and the POSIX native
endpoint seam.

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

1. Use `notes/working-todos.md` as the high-level implementation checklist.
2. Use the approved header-only `lib/dirtynet/*.hh` layout and CMake
   `INTERFACE` target shape when implementation begins.
3. Revisit portable error/result granularity after the first implementation
   pass provides concrete evidence.
4. Use the approved first implementation pass: address values, endpoint
   composition, and the POSIX native endpoint seam.
5. Finish and review the documentation-only `rev0_design` branch.
6. Create a separate implementation branch after the documentation workflow is
   resolved.
7. Implement the approved address foundation and native endpoint seam after
   Terry gives explicit approval for that specific C++ work.
8. Implement blocking caller-owned UDP bind, send, and receive behavior.
9. Validate the slice with UDP ping/pong.
10. Return to callback and async mechanics with implementation evidence.

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

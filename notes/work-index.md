# Work Index

Use this file to quickly answer: "What is the next work we should look at?"

## Quick Answer

Next work: produce a concise provisional UDP API sheet, finalize the header-only
source layout, and define the minimum portable errors required for the first
blocking UDP implementation slice.

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

1. Produce and approve a concise provisional UDP API sheet.
2. Finalize the header-only `include/dirtynet/` layout and CMake `INTERFACE`
   target shape.
3. Define minimum portable error/result values for address construction and
   blocking UDP factories and I/O.
4. Approve the first implementation capability slice.
5. Finish and review the documentation-only `rev0_design` branch.
6. Create a separate implementation branch after the documentation workflow is
   resolved.
7. Implement the address foundation and native endpoint seam after Terry gives
   explicit approval for that C++ work.
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

# Work Index

Use this file to quickly answer: "What is the next work we should look at?"

## Quick Answer

Next work: explore baseline Linux UDP and TCP socket interaction through small baseline executables, starting with UDP 1-to-1 ping/pong.

Before implementation, confirm or choose the baseline executable layout. Do not edit C++ source, headers, benchmarks, or tests until Terry explicitly asks for the specific C++ work.

## Current Work Stream

Baseline Linux socket behavior.

Goal: build understanding of raw Linux networking behavior before shaping dirtyNet abstractions.

Primary reference: `notes/plan-going-forward.md`.

Current UDP design notes: `notes/udp-pingpong-library-notes.md`.

## Ordered Task Queue

1. Decide the baseline executable layout and naming convention.
2. Add a baseline executable for UDP 1-to-1 ping/pong.
3. Add a baseline executable for TCP blocking echo server/client.
4. Add a baseline executable for a TCP framed protocol.
5. Add a baseline executable for UDP 1-to-many fanout with sequence numbers.
6. Add a baseline executable for a TCP one-thread-per-connection server.
7. Add a baseline executable for a TCP nonblocking epoll echo server.
8. Add a baseline executable for UDP request/response with timeout/retry.

## Follow-On Questions

- Which baseline examples should live under `exec/`, `sandbox/`, or another intentionally chosen area?
- Should each baseline be a single executable with modes, or separate client/server executables?
- What behavior should be documented for each baseline before turning it into a reusable dirtyNet abstraction?
- Which pieces of the UDP ping-pong boilerplate should become the first `endpoint` and `udp_socket` APIs?

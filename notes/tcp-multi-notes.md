# TCP Multi-Client Project Notes

## Current Scope

Build a blocking TCP server that accepts multiple clients and gives each
accepted connection a dedicated thread. Keep the initial application protocol
simple: echo complete lines or raw received bytes.

The baseline scaffold lives in `sandbox/tcp-multi/`. Its client and server
entry points currently validate a port argument and intentionally contain no
socket implementation.

## Learning Goals

- Observe why blocking I/O is straightforward for each individual connection.
- Identify the scheduling and memory costs of one thread per connection.
- Make shared connected-client state safe with synchronization.
- Explain context switching, scheduler pressure, and why event loops are often
  preferred for large connection counts.

## Suggested Implementation Order

1. Add the server socket lifecycle: create, bind, listen, and accept.
2. Add the client socket lifecycle and a small echo exchange.
3. Move each accepted connection into a dedicated worker thread.
4. Track the active client count safely.
5. Add a broadcast-to-all-clients command.
6. Measure rough behavior with many idle clients.

## Constraints

- Start with blocking sockets.
- Keep framing limited to a documented echo or line-based protocol.
- Keep thread ownership and shutdown behavior explicit.
- Do not turn this exercise into a reusable library abstraction yet.

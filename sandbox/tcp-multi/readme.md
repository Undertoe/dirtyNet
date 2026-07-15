# TCP Multi-Client Server: One Thread Per Connection

## Shape

- The server accepts multiple clients.
- Each accepted socket is handled by a dedicated thread.
- The protocol stays simple: echo or line-based messages.

## Exercise Goals

- Implement the socket behavior in the scaffolded client and server entry points.
- Keep blocking I/O and thread ownership easy to follow.
- Avoid adding event loops or reusable dirtyNet abstractions in this exercise.

## What It Teaches

- Blocking I/O can be simple but scales poorly.
- Thread-per-connection creates scheduling and memory overhead.
- Shared state requires synchronization.

## Good Follow-Up Tasks

- Track connected client count safely.
- Add a broadcast-to-all-clients command.
- Measure rough behavior with many idle clients.

## Interview Angle

- Explain context switching and scheduler pressure.
- Explain why event loops are commonly used for many connections.

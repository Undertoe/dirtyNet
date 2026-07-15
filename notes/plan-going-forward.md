# Plan Going Forward

## Near-Term Setup

- Keep the old implementation in `archive/legacy-dirtynet/` as a read-only reference.
- Use the new `.devcontainer/` as the cross-platform Linux development environment for Windows, macOS Intel, and macOS Apple Silicon hosts.
- Decide the new active project layout before introducing fresh C++ source files.
- Create a modern root build setup once the target layout is chosen.

## Upcoming Tasks

- Explore baseline Linux UDP and TCP socket interaction.
- Capture planned reusable features in `notes/planned-features.md` before
  promoting them into source code.
- Add a baseline executable for UDP 1-to-1 ping/pong.
- Add a baseline executable for TCP blocking echo server/client.
- Add a baseline executable for a TCP framed protocol.
- Add a baseline executable for UDP 1-to-many fanout with sequence numbers.
- Add a baseline executable for a TCP one-thread-per-connection server.
- Add a baseline executable for a TCP nonblocking epoll echo server.
- Add a baseline executable for UDP request/response with timeout/retry.

## Rebuild Direction

- Define the smallest useful dirtyNet core API before carrying over old implementation ideas.
- Start the address foundation with port and IP value types that reduce byte-order
  and IPv4/IPv6 setup ceremony.
- Use the UDP ping-pong sandbox to identify concrete boilerplate that should move behind endpoint and socket abstractions.
- Choose the active source layout, likely separating public headers, implementation, tests, benchmarks, and examples.
- Establish the baseline C++ standard, compiler warnings, sanitizers, and formatting expectations.
- Add dependency handling for Catch2 and Google Benchmark in a way that works inside the devcontainer and on host builds.
- Reintroduce tests around behavior first, using the archived tests as reference rather than blindly porting them.
- Reintroduce benchmarks only after the core abstractions settle enough to make measurements meaningful.

## Design Questions To Resolve

- Which protocols are in scope first: UDP only, TCP only, or an abstraction that can grow into both?
- What should the first address abstraction own: IPv4-only `sockaddr_in`, generic `sockaddr_storage`, or protocol-specific endpoint types?
- How should port and IP value types feed into endpoint construction?
- Should `udp_socket` expose raw POSIX-shaped calls, higher-level datagram operations, or both?
- Should protocol distinctions be compile-time types, runtime values, or a hybrid?
- What ownership model should sockets and endpoints use?
- Which async model should the project target first?
- What should count as the first complete vertical slice?

## Suggested First Milestone

Build a minimal, well-tested endpoint and socket ownership foundation with one protocol path, one benchmark harness, and a clean CMake workflow. Once that is stable, expand into reader/writer/server abstractions.

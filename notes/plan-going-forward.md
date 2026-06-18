# Plan Going Forward

## Near-Term Setup

- Keep the old implementation in `archive/legacy-dirtynet/` as a read-only reference.
- Use the new `.devcontainer/` as the cross-platform Linux development environment for Windows, macOS Intel, and macOS Apple Silicon hosts.
- Decide the new active project layout before introducing fresh C++ source files.
- Create a modern root build setup once the target layout is chosen.

## Rebuild Direction

- Define the smallest useful dirtyNet core API before carrying over old implementation ideas.
- Choose the active source layout, likely separating public headers, implementation, tests, benchmarks, and examples.
- Establish the baseline C++ standard, compiler warnings, sanitizers, and formatting expectations.
- Add dependency handling for Catch2 and Google Benchmark in a way that works inside the devcontainer and on host builds.
- Reintroduce tests around behavior first, using the archived tests as reference rather than blindly porting them.
- Reintroduce benchmarks only after the core abstractions settle enough to make measurements meaningful.

## Design Questions To Resolve

- Which protocols are in scope first: UDP only, TCP only, or an abstraction that can grow into both?
- Should protocol distinctions be compile-time types, runtime values, or a hybrid?
- What ownership model should sockets and endpoints use?
- Which async model should the project target first?
- What should count as the first complete vertical slice?

## Suggested First Milestone

Build a minimal, well-tested endpoint and socket ownership foundation with one protocol path, one benchmark harness, and a clean CMake workflow. Once that is stable, expand into reader/writer/server abstractions.

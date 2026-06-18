# Old Version Recap

The archived version of dirtyNet was left as an early C++23 networking library experiment. It is preserved at `archive/legacy-dirtynet/`.

## Layout

- `core/` built a shared library named `dirtyNet` from every `.cc` file in the directory.
- `tests/` built `dirtyNet_test` from Catch2 v2 test files and linked against the core library.
- `benchmarks/` built `dirtyNet_bench` with Google Benchmark.
- The root `CMakeLists.txt` used FetchContent for Catch2 v2.13.10 and expected Google Benchmark to be available through pkg-config / `find_package`.
- The root `makefile` wrapped CMake build, test, benchmark, GCC 13, Clang 16, and macOS benchmark flows.

## Implemented Areas

The archived source tree contains work around:

- IP abstractions: `ip`, `ipv4`, and `ipv6`.
- Endpoint modeling.
- Socket file descriptor ownership.
- UDP-oriented socket work.
- Backend tags and backend trait experiments.
- Utility and vector view helpers.
- Tests for the above areas plus some STL experiments.
- Benchmark experiments for endpoints and benchmark harness setup.

## Last Known Direction

The archived README described dirtyNet as a "Clean and dirty Network Library" and sketched larger goals around:

- Server interfaces for one-to-many connections with asynchronous reads and targeted writes.
- Writer and reader connection types that may be persistent or single-use, sync or async.
- General purpose sockets for persistent read/write connections.
- An eventual HTTP socket layer with initial GET, PUT, and POST support.
- Protocol-specific connection objects, likely distinguishing TCP and UDP at compile time.

## Historical Constraints

The old `.devcontainer` was a git submodule pointing at `https://github.com/Undertoe/.devcontainer.git`. That workflow has been intentionally removed.

Treat this archive as reference material, not as the active implementation.

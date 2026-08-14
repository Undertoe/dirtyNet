# dirtyNet

This repository is being reset around a fresh C++ project structure.

The previous implementation has been preserved under `archive/legacy-dirtynet/` for historical reference. The old devcontainer submodule and `.gitmodules` file have been removed.

Current top-level layout:

- `archive/legacy-dirtynet/` - old source tree, tests, benchmarks, and build files preserved as reference.
- `lib/` - active dirtyNet library target and public headers.
- `tests/` - Catch2-based tests that link against the library.
- `perf/` - Google Benchmark performance targets that link against the library.
- `exec/` - sample executables that demonstrate library use.
- `sandbox/` - small exploratory executables, including standalone and library-linked examples.
- `notes/` - recap of the archived version and forward plan for the rebuild.
- `.devcontainer/` - new architecture-neutral Ubuntu development container setup.

The archived C++ files should be treated as read-only reference material unless a future task explicitly asks to modify them.

## Build

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Use `-DDIRTYNET_BUILD_PERF=OFF` to skip Google Benchmark targets.



## AI Note: 
As a note, no AI was used in the generation of our source code.  AI is priamrely used for boilerplate cmake generation, dev environment setup, and as a very handy rubber duck when stuck.  

# Task 0001 — CMake project skeleton & directory layout

**Milestone:** M0 — Bootstrap & CI
**Estimated Effort:** 0.75 hours
**Depends on:** none

## Introduction (for project managers)

Before any feature can be written we need a place to put it. This task creates
the empty house: the folder structure, the top-level build file, and a tiny
program that compiles and runs. Nothing user-facing yet, but every later task
plugs into this skeleton, so getting the layout right now saves churn later.

## Detailed Design (for engineers)

Create the canonical source tree that mirrors the dependency layering in
`CLAUDE.md` (Application -> Domain -> Interfaces -> Infrastructure -> Third Party):

```
raptor/
  CMakeLists.txt            # top-level project(), C++23, sub-includes
  cmake/                    # helper modules (warnings, sanitizers)
  include/raptor/           # public headers, mirrors namespaces below
  src/
    domain/                 # pure logic, no third-party includes
    interfaces/             # abstract interfaces only
    infrastructure/         # adapters wrapping third-party libs
    application/            # engine/session wiring
    cli/                    # main()
  tests/                    # GTest tree mirroring src/
  design/                   # task files (this folder)
```

Top-level `CMakeLists.txt`: `cmake_minimum_required(VERSION 3.28)`,
`project(raptor CXX)`, set `CMAKE_CXX_STANDARD 23`,
`CMAKE_CXX_STANDARD_REQUIRED ON`, `CMAKE_CXX_EXTENSIONS OFF`. Add an
`add_library(raptor_core)` target (initially with one placeholder `.cpp`) and an
`add_executable(raptor)` in `src/cli`. Create a `cmake/Warnings.cmake` that
defines an INTERFACE target `raptor_warnings` with `-Wall -Wextra -Wpedantic`
(`/W4` on MSVC) and link it privately to all Raptor targets. Do not add
dependencies here — that is task 0003.

## Acceptance Criteria

- Fresh clone configures and builds with `cmake -S . -B build && cmake --build build` on Linux, macOS and Windows.
- `raptor_core` library and `raptor` executable targets both build.
- Folder layout matches the dependency layering above; `domain/` contains no third-party includes.
- C++23 is enforced (`CMAKE_CXX_STANDARD 23`, extensions off).

## Testing Strategy

No unit tests for the skeleton itself. Verification is the successful configure
and build on all three host OSes (covered once CI lands in 0006). A trivial
`raptor --version`-style stub returning 0 confirms the executable links.

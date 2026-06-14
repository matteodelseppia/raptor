# Task 0002 — CMake configure/build/test presets per toolchain

**Milestone:** M0 — Bootstrap & CI
**Estimated Effort:** 0.75 hours
**Depends on:** 0001

## Introduction (for project managers)

We promised the same build commands work for every developer and every CI
machine. CMake "presets" are named, version-controlled build configurations so
nobody has to remember long command lines, and CI runs exactly what developers
run locally. This removes "works on my machine" surprises.

## Detailed Design (for engineers)

Add a `CMakePresets.json` (schema version >= 6). Define a hidden `base`
configure preset using the Ninja generator, `build/${presetName}` binary dir,
and `CMAKE_EXPORT_COMPILE_COMMANDS=ON` (needed by clang-tidy). Derive concrete
configure presets:

- `gcc-debug`, `gcc-release` (sets `CC=gcc-16 CXX=g++-16` via `cacheVariables`/`environment`).
- `clang-debug`, `clang-release` (`clang-22`/`clang++-22`).
- `appleclang-debug`/`appleclang-release` (no compiler override, default Xcode toolchain).
- `msvc-debug`/`msvc-release` (generator `Visual Studio 17 2022` or Ninja with the VS env).

Add matching `buildPresets` and `testPresets` (the latter with
`output.outputOnFailure: true`). Add a `dev` configure preset that enables
sanitizers (ASan+UBSan) on the Debug build via a cache option `RAPTOR_SANITIZE`.
Document the preset names in `design/README.md` (task 0055 index references them).

## Acceptance Criteria

- `cmake --preset gcc-debug` (and each other preset) configures successfully on the matching platform.
- `cmake --build --preset gcc-debug` builds; `ctest --preset gcc-debug` runs.
- Presets pin the required compilers (gcc-16, clang-22) via cache/env vars.
- `compile_commands.json` is generated in each build dir.

## Testing Strategy

Validation is configure+build success per preset, exercised by CI in 0006.
Locally, `cmake --list-presets` must show all presets without schema errors.

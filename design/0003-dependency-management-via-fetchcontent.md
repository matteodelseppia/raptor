# Task 0003 — Dependency management via FetchContent

**Milestone:** M0 — Bootstrap & CI
**Estimated Effort:** 1 hour
**Depends on:** 0001, 0002

## Introduction (for project managers)

Raptor needs a few well-known open-source libraries (for logging, testing,
networking, command-line parsing). Instead of asking every developer to install
them manually, the build downloads and compiles pinned versions automatically.
This guarantees everyone — and CI — uses the exact same versions.

## Detailed Design (for engineers)

Create `cmake/Dependencies.cmake` using `FetchContent` with **pinned git tags**
and `FetchContent_MakeAvailable`. Set `FETCHCONTENT_UPDATES_DISCONNECTED=ON` for
faster reconfigure. Initial dependencies:

- `spdlog` (logging adapter, infrastructure only).
- `googletest` (GTest + GMock; gate behind `if(RAPTOR_BUILD_TESTS)`).
- `CLI11` (argument parsing, used in task 0053).
- `asio` (standalone, header-only; networking runtime, task 0020).

Each `FetchContent_Declare` pins `GIT_TAG <release-tag>` (not a branch) and sets
`GIT_SHALLOW ON`. Wrap third-party warnings out (`SYSTEM` includes) so they do
not trip `-Werror`/clang-tidy. Expose an option `RAPTOR_BUILD_TESTS` (default ON
for top-level builds, OFF when consumed as a subproject). Only
`infrastructure/` targets may link these libs directly, preserving the
dependency rule that `domain/` stays third-party-free.

## Acceptance Criteria

- A clean build with no system-installed libraries succeeds (deps fetched automatically).
- All dependency versions are pinned to immutable git tags.
- Third-party headers are included as SYSTEM (no warnings leak into Raptor build).
- `RAPTOR_BUILD_TESTS=OFF` skips fetching googletest.

## Testing Strategy

Build-time verification only. CI cold cache build (0006) proves fetch works
from scratch. A smoke test target linking spdlog and asio confirms they resolve.

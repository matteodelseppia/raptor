# Task 0005 — clang-tidy configuration and lint check

**Milestone:** M0 — Bootstrap & CI
**Estimated Effort:** 0.75 hours
**Depends on:** 0002, 0003

## Introduction (for project managers)

A linter catches bug-prone patterns and enforces our modern-C++ style
automatically. Wiring it in early keeps quality high without manual policing in
code review.

## Detailed Design (for engineers)

Add a `.clang-tidy` enabling pragmatic check groups:
`bugprone-*, performance-*, modernize-*, cppcoreguidelines-* (selected),
readability-* (selected)`, with explicit disables for noisy checks
(`modernize-use-trailing-return-type`, overly strict `cppcoreguidelines` magic
numbers, etc.). Set `WarningsAsErrors` for the curated subset. clang-tidy
consumes `compile_commands.json` (already emitted via preset, task 0002).
Add `cmake/ClangTidy.cmake` providing a `tidy` target that runs
`run-clang-tidy -p build/<preset>` over `src/` and `include/` only (not tests,
not fetched deps). Require clang-tidy >= 18. Keep `HeaderFilterRegex` scoped to
`include/raptor/` and `src/`.

## Acceptance Criteria

- `tidy` target runs clang-tidy across first-party sources using compile_commands.json.
- Curated check set is treated as errors; the clean skeleton passes.
- Fetched dependencies and tests are excluded from analysis.
- `.clang-tidy` committed at repo root.

## Testing Strategy

Verification is a passing run on the current tree plus a forced failure: insert
a `modernize-use-nullptr`-triggering `NULL` and confirm `tidy` reports it. CI
step in 0006 runs `tidy` on at least the Clang job.

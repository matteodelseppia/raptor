# Task 0007 — First executable, CTest integration and smoke test

**Milestone:** M0 — Bootstrap & CI
**Estimated Effort:** 0.5 hours
**Depends on:** 0001, 0002, 0003

## Introduction (for project managers)

We need at least one real test so CI has something meaningful to run from the
first commit. This task adds the testing plumbing and a trivial test, proving
the GTest/CTest pipeline works end to end before real features arrive.

## Detailed Design (for engineers)

In `tests/CMakeLists.txt`, create a `raptor_tests` executable linking GTest/GMock
(from 0003) and `raptor_core`. Use `gtest_discover_tests` (via
`include(GoogleTest)`) so each test registers with CTest. Add `tests/smoke_test.cpp`
with a single `TEST(Smoke, BuildLinks)` asserting a version constant from
`raptor_core` (e.g. `raptor::version()` returns a non-empty `std::string_view`).
Wire `enable_testing()` at top level. Ensure tests build only when
`RAPTOR_BUILD_TESTS=ON`.

## Acceptance Criteria

- `ctest --preset <toolchain>-debug` discovers and runs the smoke test, which passes.
- Tests are excluded when `RAPTOR_BUILD_TESTS=OFF`.
- `gtest_discover_tests` registers individual tests with CTest.

## Testing Strategy

The smoke test is the test. It must be deterministic and offline (it is). Serves
as the canary that linking, discovery, and CTest invocation all work in CI.

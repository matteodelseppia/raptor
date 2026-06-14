# Task 0006 — GitHub Actions CI matrix across all toolchains

**Milestone:** M0 — Bootstrap & CI
**Estimated Effort:** 1 hour
**Depends on:** 0002, 0003, 0004, 0005, 0007

## Introduction (for project managers)

This is the safety net the whole request is about: every push and pull request
is automatically built and tested on all four target toolchains, and rejected if
formatting or lint fails. After this task, no broken or unformatted code can
merge unnoticed. We want this live as early as possible so all later tasks are
covered.

## Detailed Design (for engineers)

Add `.github/workflows/ci.yml` triggered on `push` and `pull_request`. Use a
`strategy.matrix` with four jobs:

- `linux-gcc16`: `ubuntu-24.04`, install gcc-16/g++-16 (toolchain PPA or apt), Ninja.
- `linux-clang22`: `ubuntu-24.04`, install clang-22/clang++-22 + clang-format/clang-tidy 22.
- `macos-arm64`: `macos-15` (Apple Silicon runner), default AppleClang + Ninja.
- `windows-msvc`: `windows-2022`, MSVC 19.4x via `ilammy/msvc-dev-cmd` or built-in.

Each job: checkout, set up Ninja, run `cmake --preset <toolchain>-release`,
`cmake --build --preset <toolchain>-release`, then `ctest --preset
<toolchain>-release --output-on-failure`. Add two **separate lint jobs** that run
once (on the clang image): `format-check` (task 0004) and `tidy` (task 0005), so
formatting/lint failures block merge independently of platform builds. Cache
FetchContent downloads with `actions/cache` keyed on the dependency-pin file
hash. Mark all jobs required for merge via branch protection (documented in
`design/README.md`).

## Acceptance Criteria

- All four build/test jobs are green on the skeleton + first test (task 0007).
- A formatting violation fails the `format-check` job; a clang-tidy violation fails the `tidy` job.
- CI runs on every push and PR.
- Dependency downloads are cached between runs.
- Required compiler versions match: GCC 16, Clang 22, AppleClang (macOS arm64), MSVC 19.

## Testing Strategy

Self-verifying: the workflow itself is the test. Open a draft PR with (a) a
formatting error and (b) a tidy error to confirm the lint jobs fail, then revert.
Confirm all matrix legs pass on the baseline commit.

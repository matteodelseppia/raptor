# Task 0004 — clang-format config and format-check target

**Milestone:** M0 — Bootstrap & CI
**Estimated Effort:** 0.5 hours
**Depends on:** 0001

## Introduction (for project managers)

Consistent code formatting removes pointless debate and noisy diffs. We adopt an
automatic formatter and make CI reject any code that is not formatted. This must
exist from day one so the codebase never accumulates formatting drift.

## Detailed Design (for engineers)

Add a `.clang-format` at repo root (BasedOnStyle: LLVM or Google — pick one,
ColumnLimit 100, `Standard: c++20`+ acceptable for c++23 constructs). Add a
`.clang-format-ignore` (or rely on path filters) to exclude `build/` and fetched
content. Add a CMake custom target `format` (applies formatting in place) and
`format-check` (runs `clang-format --dry-run --Werror` over
`src/`, `include/`, `tests/`). Provide a small `cmake/ClangFormat.cmake` that
globs tracked sources. Require clang-format version >= 18 to match CI image.

## Acceptance Criteria

- `cmake --build build --target format-check` exits non-zero if any tracked C++ file is unformatted.
- `format` target reformats in place.
- `.clang-format` is committed and applies to all `src/include/tests` files.
- Generated/fetched code is excluded.

## Testing Strategy

Add a deliberately misformatted fixture under `tests/format_fixture` (excluded
from compilation) is NOT needed; instead CI step in 0006 runs `format-check`
against the real tree. Locally verify by introducing bad spacing and confirming
non-zero exit.

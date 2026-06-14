# Task 0037 — Random scheduler

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.5 hours
**Depends on:** 0031

## Introduction (for project managers)

Requests needed pieces in a random order. Helpful for benchmarking and as a
control when evaluating smarter strategies. Listed as a supported mode.

## Detailed Design (for engineers)

Implement `RandomPicker : IPiecePicker` choosing uniformly among needed,
peer-available pieces using an **injected, seedable RNG** (so tests are
deterministic — never use a global random engine). Register under
`--piece-order random`.

## Acceptance Criteria

- Selects uniformly among eligible pieces using an injected seedable RNG.
- With a fixed seed, output is reproducible.
- Passes the shared picker test suite.
- Selectable via `--piece-order random`.

## Testing Strategy

Unit tests with a seeded RNG: assert reproducible selection sequence; assert all
needed pieces are eventually chosen; never picks owned/unavailable. Deterministic
via seed.

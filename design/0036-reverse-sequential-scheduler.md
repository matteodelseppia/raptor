# Task 0036 — Reverse-sequential scheduler

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.25 hours
**Depends on:** 0031, 0035

## Introduction (for project managers)

Same as sequential but from the last piece backward. The README lists it as a
supported mode (useful for certain streaming/container formats).

## Detailed Design (for engineers)

Implement `ReversePicker : IPiecePicker` selecting the highest-index needed,
peer-available piece. Largely mirrors 0035 with inverted ordering; consider
sharing a base. Register under `--piece-order reverse`.

## Acceptance Criteria

- Selects the highest-index needed, available piece next.
- Passes the shared picker test suite.
- Selectable via `--piece-order reverse`.

## Testing Strategy

Unit tests: assert strictly decreasing request order; shared picker suite.
Deterministic.

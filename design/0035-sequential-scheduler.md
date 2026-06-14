# Task 0035 — Sequential scheduler

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.5 hours
**Depends on:** 0031

## Introduction (for project managers)

The simplest strategy: download pieces in order from first to last. Useful for
streaming-style use and as a correctness baseline. The README lists it as a
supported mode.

## Detailed Design (for engineers)

Implement `SequentialPicker : IPiecePicker` returning the lowest-index needed
piece available from the given peer, then its lowest-offset missing block.
Integrates with `RequestTracker` (0032) for in-flight awareness. Register in the
factory under `--piece-order sequential`.

## Acceptance Criteria

- Always selects the lowest-index needed, peer-available piece next.
- Passes the shared picker test suite (no re-request of owned pieces, full coverage).
- Selectable via `--piece-order sequential`.

## Testing Strategy

Unit tests: with a scripted availability map, assert the exact request order is
strictly increasing by piece index. Run the shared parametrized picker suite.
Deterministic.

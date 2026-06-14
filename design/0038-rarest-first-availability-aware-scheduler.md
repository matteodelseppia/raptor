# Task 0038 — Rarest-first / availability-aware scheduler

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.75 hours
**Depends on:** 0030, 0031

## Introduction (for project managers)

A cornerstone BitTorrent strategy: fetch the pieces that fewest peers have first,
to maximize the chance every piece stays obtainable and to spread load. This
directly serves the project's goal of efficient piece acquisition.

## Detailed Design (for engineers)

Implement `RarestFirstPicker : IPiecePicker` using `PieceAvailability` (0030):
order needed pieces by ascending availability, breaking ties randomly (seedable
RNG) to avoid all peers converging on the same piece. Maintain an incremental
rarest ordering updated on availability changes for efficiency (avoid full
re-sort per request). Optionally apply a "random first piece" warm-up. Register
under `--piece-order` availability/rarest.

## Acceptance Criteria

- Prefers lowest-availability needed pieces; ties broken randomly with a seedable RNG.
- Ordering updates incrementally as Have/Bitfield/disconnect change availability.
- Passes the shared picker test suite.

## Testing Strategy

Unit tests: scripted availability -> assert rarest piece chosen first; update
availability and assert order shifts; tie-break reproducibility with fixed seed.
Benchmark scenario shows fewer "stuck" pieces vs sequential. Deterministic.

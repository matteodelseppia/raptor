# Task 0032 — Block request tracking (in-flight, timeouts)

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.75 hours
**Depends on:** 0010, 0019, 0031

## Introduction (for project managers)

Pieces are downloaded in small blocks requested from peers. Raptor must track
which blocks it has asked for, from whom, and re-request ones that never arrive —
otherwise a slow peer can stall a piece forever. This task is the bookkeeping that
keeps requests flowing.

## Detailed Design (for engineers)

Implement `RequestTracker`: records outstanding `BlockRequest{piece, begin,
length, peer, deadline}`; supports `markRequested`, `markReceived`,
`expire(now)` (clock-driven) returning timed-out blocks for re-request, and
per-peer in-flight caps (pipelining depth, e.g. derived from `--max-requests`).
Prevent duplicate requests of the same block except in endgame mode (0040).
Provide queries used by the picker (which blocks of a piece remain).

## Acceptance Criteria

- Tracks in-flight blocks per peer with a configurable pipeline depth.
- Expires timed-out requests (clock-driven) and offers them for re-request.
- Prevents duplicate in-flight blocks outside endgame mode.
- Reports remaining blocks per partially-downloaded piece.

## Testing Strategy

Unit tests with `FakeClock`: request/receive flow, timeout expiry after advancing
the clock, pipeline cap enforcement, duplicate-suppression. Deterministic.

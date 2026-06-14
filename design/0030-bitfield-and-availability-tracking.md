# Task 0030 — Bitfield and availability tracking

**Milestone:** M6 — Peer protocol
**Estimated Effort:** 0.5 hours
**Depends on:** 0012, 0019

## Introduction (for project managers)

Each peer advertises which pieces it has. Raptor aggregates this across all peers
to know how rare each piece is — information the smart schedulers use to grab
scarce pieces first. This task maintains those data structures.

## Detailed Design (for engineers)

Implement `Bitfield` (compact bit storage with `set/test/count`, MSB-first per
BitTorrent spec, length = piece_count rounded to bytes; reject spare bits set).
Implement `PieceAvailability`: aggregate counts of how many connected peers have
each piece, updated on `Bitfield`/`Have` and on peer disconnect (decrement).
Provide queries `availability(index)` and `rarestOrder()` used by the
rarest-first scheduler (0038).

## Acceptance Criteria

- `Bitfield` enforces MSB-first ordering and validates length/spare bits.
- `PieceAvailability` increments on Have/Bitfield and decrements on disconnect, never going negative.
- Provides per-piece availability and a rarest-first ordering.

## Testing Strategy

Unit tests: bitfield set/test against reference byte patterns, spare-bit
rejection; availability counts after simulated peers join/leave; rarest ordering
correctness with ties. Deterministic.

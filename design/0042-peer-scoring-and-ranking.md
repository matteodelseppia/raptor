# Task 0042 — Peer scoring and ranking

**Milestone:** M8 — Peer strategy
**Estimated Effort:** 0.5 hours
**Depends on:** 0030, 0041

## Introduction (for project managers)

Using the measurements, Raptor ranks peers so it can favour the good ones and cut
the dead weight. This ranking drives which peers we keep, unchoke, and request
from — central to the "adaptive peer management" the README advertises.

## Detailed Design (for engineers)

Implement `PeerScorer` producing a scalar score from a `PeerMetrics` snapshot
plus piece-availability usefulness (0030): weight download rate highest, then
reliability and how many *needed* pieces the peer can provide; penalize peers
that keep us choked with no data. Expose a comparator/ranking and a configurable
weight set (injectable for experimentation). Pure function of inputs (testable).

## Acceptance Criteria

- Produces a deterministic score from metrics + availability.
- Ranks fast/reliable/useful peers above slow/unreliable/idle ones.
- Weights are configurable and injected.

## Testing Strategy

Unit tests: construct peers with controlled metrics/availability, assert ranking
order matches expectations; assert a fast-but-useless (no needed pieces) peer is
ranked appropriately; weight changes shift ranking predictably. Deterministic.

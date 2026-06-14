# Task 0041 — Peer performance metrics

**Milestone:** M8 — Peer strategy
**Estimated Effort:** 0.5 hours
**Depends on:** 0010, 0029

## Introduction (for project managers)

To prefer the peers that help us finish fastest, we first have to measure each
peer. This task records how fast each peer sends us data and how reliable it is —
the raw inputs for all peer decisions.

## Detailed Design (for engineers)

Implement `PeerMetrics` per connection: rolling download rate (bytes/sec via an
EWMA or fixed window over `IClock`), upload sent, count of completed vs
timed-out/failed blocks (reliability), choke/unchoke history, and time-connected.
Update from `PeerConnection` events (0029) and `RequestTracker` (0032). Provide a
concise snapshot struct for scoring (0042) and stats (0051). Log per-peer rate
periodically per logging guidance.

## Acceptance Criteria

- Computes a rolling download rate from received-block events and the clock.
- Tracks reliability (completed vs failed/timed-out blocks) and upload volume.
- Exposes an immutable snapshot for scoring/stats.

## Testing Strategy

Unit tests with `FakeClock`: feed timestamped byte deltas, assert EWMA/windowed
rate matches hand-computed values; reliability ratio after mixed success/timeout
events. Deterministic.

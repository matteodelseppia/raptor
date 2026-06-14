# Task 0051 — Progress and performance statistics reporting

**Milestone:** M11 — Session
**Estimated Effort:** 0.5 hours
**Depends on:** 0010, 0041, 0050

## Introduction (for project managers)

Users want to see what's happening: download speed, percent complete, connected
peers, and an estimated time to finish. This task computes and reports those
stats — the README's "detailed performance statistics".

## Detailed Design (for engineers)

Implement `StatsReporter` aggregating: overall download/upload rate (from
0041), pieces verified / total, percent complete, connected/unchoked peer counts,
and an ETA derived from remaining bytes and a smoothed rate (clock-based). Emit a
periodic, concise structured log line (per logging guidance) and expose a
snapshot struct for the CLI to render a progress line. Avoid log spam (rate-limit
to the tick interval).

## Acceptance Criteria

- Reports rate, percent complete, peer counts, and a smoothed ETA.
- Emits concise periodic updates without spamming logs.
- Snapshot is consumable by the CLI for live progress display.

## Testing Strategy

Unit tests with `FakeClock` + scripted progress: assert percent/ETA computation
against hand-calculated values; ETA stabilizes as rate smooths; no update emitted
more than once per interval. Deterministic.

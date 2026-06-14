# Task 0039 — Optimized default scheduler

**Milestone:** M7 — Scheduling
**Estimated Effort:** 1 hour
**Depends on:** 0032, 0038, 0040

## Introduction (for project managers)

This is Raptor's flagship strategy and default mode — the one tuned purely for
finishing fastest. It blends rarest-first with peer speed awareness so fast peers
are kept busy on pieces that unblock the most progress. The README markets this
as the headline feature.

## Detailed Design (for engineers)

Implement `OptimizedPicker : IPiecePicker` combining: (1) rarest-first as the
base ordering (0038); (2) **peer-throughput awareness** — assign in-progress
pieces to fast peers and avoid splitting a near-complete piece across slow peers
(reduces time-to-first-verified-piece); (3) prefer completing already-started
pieces over opening new ones (limits fragmentation); (4) trigger endgame (0040)
when few blocks remain. Keep tunables (`--max-requests`, peer-speed weighting)
injectable. Document the heuristic's intent/trade-offs in Doxygen per CLAUDE.md.
Register as the default `--piece-order optimized`.

## Acceptance Criteria

- Default picker; combines rarest-first, started-piece preference, and peer-speed assignment.
- Hands near-complete pieces to the fastest available peers.
- Hands off to endgame mode when remaining blocks fall below a threshold.
- Passes the shared picker suite and beats sequential/random on the completion-time benchmark scenario.

## Testing Strategy

Deterministic scenario simulations (fixed peer speed/availability scripts)
asserting: started pieces preferred, fast peers get scarce/near-complete pieces,
endgame engages near the end. Comparative benchmark vs other pickers reports a
lower simulated completion metric. Shared picker suite. Deterministic.

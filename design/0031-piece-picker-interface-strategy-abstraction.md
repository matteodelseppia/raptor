# Task 0031 — Piece picker interface (strategy abstraction)

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.75 hours
**Depends on:** 0019, 0030

## Introduction (for project managers)

The order in which Raptor requests pieces is its core competitive advantage and
the project explicitly wants to experiment with different strategies. This task
defines the plug-in seam so any strategy (sequential, random, rarest-first, the
optimized default) drops in interchangeably.

## Detailed Design (for engineers)

Define `class IPiecePicker` in `interfaces/`: given the set of pieces we still
need, per-peer availability (0030), and which peer we're choosing for, return the
next piece(s)/blocks to request. Key methods: `nextRequests(PeerView,
maxRequests) -> std::vector<BlockRequest>`, plus `onPieceCompleted`,
`onPieceFailed`, `onPeerAvailabilityChanged` hooks. Provide a `SchedulerKind`
enum and a `makePicker(kind, ...)` factory mapping CLI `--piece-order` values to
implementations. Define a stable test harness/`SchedulerScenario` so each
concrete scheduler (0035–0039) is benchmarkable, per CLAUDE.md's
"scheduler performance measurable" requirement.

## Acceptance Criteria

- `IPiecePicker` cleanly separates strategy from the rest of the engine.
- A factory maps `--piece-order` values (optimized/sequential/reverse/random) to implementations.
- A reusable scenario harness can drive any picker deterministically and report a completion-order metric.
- Adding a new scheduler requires only a new class + factory entry.

## Testing Strategy

Define a shared parametrized test suite all pickers must pass (never request a
piece we already have; respect availability; eventually request every needed
piece). Provide a `MockPicker` for engine tests. The scenario harness produces a
deterministic, comparable metric. Deterministic.

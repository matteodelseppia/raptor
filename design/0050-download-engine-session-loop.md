# Task 0050 — Download engine / session loop

**Milestone:** M11 — Session
**Estimated Effort:** 1 hour
**Depends on:** 0025, 0031, 0032, 0033, 0034, 0044, 0049

## Introduction (for project managers)

This is the conductor that brings every component together into a working
download: start the tracker, manage peers, pick pieces, request and verify
blocks, write to disk, and track progress to completion. After this task Raptor
can actually download a torrent.

## Detailed Design (for engineers)

Implement `DownloadSession` wiring: `TrackerClient` (0025) ->
`ConnectionManager` (0044) -> per-peer message handling (0029) feeding the
`IPiecePicker` (0031) + `RequestTracker` (0032); received blocks ->
`PieceAssembler` (0033) -> `Storage` (0034); verified pieces update availability,
broadcast `Have`, and persist resume (0048). Run on the `IoRuntime` (0020) with a
periodic tick driving choke control (0043), request expiry (0032), and stats
(0051). Detect completion (all needed pieces verified) and transition to the
completion handler (0052). Clean cancellation/shutdown that saves resume state.

## Acceptance Criteria

- Orchestrates tracker, peers, picker, requests, verification, and storage into a working download.
- Periodic tick drives choking, request timeouts, and stats.
- Detects completion of the needed-piece set and transitions to completion handling.
- Clean shutdown persists resume state.

## Testing Strategy

Integration test on a **local loopback fake swarm**: an in-test seeder serving a
small generated torrent's pieces over the real peer protocol; assert the session
downloads and verifies the whole torrent and reconstructs files byte-identically.
A fully mocked variant (fake peers/clock) tests orchestration deterministically.
No internet (CLAUDE.md compliant — loopback only).

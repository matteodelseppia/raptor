# Task 0040 — Endgame mode

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.5 hours
**Depends on:** 0032, 0033

## Introduction (for project managers)

At the very end of a download, a few last blocks are often stuck behind one slow
peer. Endgame mode requests those final blocks from several peers at once and
cancels the duplicates as soon as one arrives — eliminating the frustrating
"stuck at 99%" tail.

## Detailed Design (for engineers)

Add endgame behaviour: when remaining needed blocks <= threshold, allow the
`RequestTracker` (0032) to issue the same block to multiple peers. On first
receipt of a block, send `Cancel` (0026) to the other peers holding the duplicate
request and clear them from the tracker. Expose enable threshold; integrate with
the optimized picker (0039).

## Acceptance Criteria

- Activates only when remaining blocks are below the configured threshold.
- Permits duplicate in-flight requests across peers during endgame.
- Sends `Cancel` to redundant peers upon first block receipt and clears duplicates.

## Testing Strategy

Unit tests: simulate end-of-download with one slow + one fast peer; assert
duplicate requests issued, fast block received, `Cancel` sent to the slow peer,
duplicate cleared. Deterministic with FakeClock/scripted peers.

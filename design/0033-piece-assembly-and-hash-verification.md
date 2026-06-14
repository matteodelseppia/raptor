# Task 0033 — Piece assembly and hash verification

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.5 hours
**Depends on:** 0016, 0019, 0032

## Introduction (for project managers)

As blocks arrive they're assembled into a full piece, then checked against the
hash from the .torrent file. Only verified pieces count as downloaded; corrupt
ones are thrown away and re-fetched. This guarantees the final files are correct.

## Detailed Design (for engineers)

Implement `PieceAssembler`: buffers received blocks for an in-progress piece,
detects completion, computes SHA-1 (via `IHasher`, 0016) over the assembled
piece, and compares to the expected `Hash20` from `PieceLayout`/metainfo. On
match -> emit a verified piece (to storage 0034) and mark complete (0030
availability/`have`). On mismatch -> discard buffer, return `HashMismatch`, and
let the tracker (0032) re-request. Manage buffer memory (free after write).

## Acceptance Criteria

- Assembles blocks in order regardless of arrival order; detects piece completion.
- Verifies SHA-1 against the expected digest; accepts matches, rejects mismatches.
- On mismatch, discards the piece and signals for re-download.
- Frees block buffers after a verified write.

## Testing Strategy

Unit tests with `FakeHasher`/real hasher: assemble out-of-order blocks ->
complete -> verify pass; corrupt one block -> verify fail and discard. Edge:
last (short) piece. Deterministic.

# Task 0046 — Map selected files to required pieces

**Milestone:** M9 — Selective downloads
**Estimated Effort:** 0.5 hours
**Depends on:** 0019, 0045

## Introduction (for project managers)

Because pieces can straddle file boundaries, choosing files isn't the same as
choosing pieces — a piece at the edge of a wanted file also contains bytes of an
unwanted neighbour. This task computes exactly which pieces are needed so Raptor
downloads only what's required (the README's "only the required pieces").

## Detailed Design (for engineers)

Implement `requiredPieces(FileSelection, PieceLayout) -> set<piece_index>`:
include every piece overlapping any selected file's byte range (boundary pieces
shared with unselected files are still required). Feed this set to the piece
picker as the "needed" universe and to storage (0034) so unselected-only files
aren't allocated. Note partial boundary pieces mean small amounts of unselected
data are downloaded — document this.

## Acceptance Criteria

- Produces the minimal correct piece set covering all selected files, including shared boundary pieces.
- Pieces wholly within unselected files are excluded.
- Integrates as the picker's needed-set and informs storage allocation.

## Testing Strategy

Unit tests with multi-file fixtures: select one middle file, assert boundary
pieces included and interior unrelated pieces excluded; select non-contiguous
files; whole-torrent selection equals all pieces. Deterministic.

# Task 0019 — Piece layout model

**Milestone:** M3 — Metainfo
**Estimated Effort:** 0.5 hours
**Depends on:** 0012

## Introduction (for project managers)

Downloads are split into fixed-size "pieces", each verified independently. This
small model answers the questions every later component asks: how big is piece N,
which bytes of which files does it cover, how many pieces are there. Centralizing
this math avoids subtle off-by-one bugs.

## Detailed Design (for engineers)

Define `PieceLayout` (domain): `piece_length`, `total_length`, `piece_count`,
and methods `pieceSize(index)` (last piece is the remainder), `blockCount(index)`
for a given block size (typically 16 KiB), and a mapping
`pieceToFileRegions(index) -> span of {file_index, file_offset, length}` for
writing into multi-file torrents. Pure value type, no I/O. Reused by storage
(0034), verification (0033), and schedulers (M7).

## Acceptance Criteria

- `pieceSize` returns piece_length for all but the last piece, correct remainder for the last.
- `blockCount` handles partial final blocks.
- `pieceToFileRegions` correctly splits pieces spanning file boundaries.
- Pure, deterministic, no I/O.

## Testing Strategy

Unit tests covering: exact-multiple vs remainder total lengths; single-file and
multi-file region mapping including a piece straddling two files; block counts
for full and partial pieces. Table-driven, deterministic.

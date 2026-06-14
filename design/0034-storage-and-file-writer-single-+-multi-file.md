# Task 0034 — Storage and file writer (single + multi-file)

**Milestone:** M7 — Scheduling
**Estimated Effort:** 0.75 hours
**Depends on:** 0011, 0019, 0033

## Introduction (for project managers)

Verified pieces have to be written to the right place on disk — and for
multi-file torrents a single piece can span several files. This task writes data
correctly into the user's files, including pre-allocating space and creating the
folder structure.

## Detailed Design (for engineers)

Implement `Storage` over `IFileSystem` (0011): on init, create the directory
tree and (sparse-)allocate each file at its declared size. `writePiece(index,
bytes)` uses `PieceLayout::pieceToFileRegions` (0019) to split the piece across
file boundaries and write each region at the correct offset. Provide
`readPiece(index)` for resume reverification (0049). Respect selective-download
(0046): skip writes for unselected files. Flush/sync policy documented.

## Acceptance Criteria

- Initializes the full file/dir layout with correct sizes (sparse where supported).
- `writePiece` correctly splits boundary-spanning pieces across multiple files.
- `readPiece` reads data back identically (round-trip).
- Unselected files are not allocated/written when selective download is active.

## Testing Strategy

Unit tests against `InMemoryFileSystem` (0011): write all pieces of a multi-file
layout, read back, assert byte-identical reconstruction; verify a boundary-
spanning piece lands in two files at correct offsets. Parity test against
`StdFileSystem` in a temp dir. Deterministic, offline.

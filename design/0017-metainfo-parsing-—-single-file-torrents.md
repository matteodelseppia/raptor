# Task 0017 — Metainfo parsing — single-file torrents

**Milestone:** M3 — Metainfo
**Estimated Effort:** 0.75 hours
**Depends on:** 0014, 0019

## Introduction (for project managers)

A .torrent file describes what to download: file names, sizes, piece size, and
the tracker address. This task extracts that information for the common
single-file case, producing the structured "torrent description" the rest of
Raptor works from.

## Detailed Design (for engineers)

Define `Metainfo` (domain): `announce` URL, optional `announce-list`,
`info.name`, `info.piece_length`, the concatenated `pieces` hashes split into
`std::vector<Hash20>`, and for single-file mode a `length`. Implement
`Result<Metainfo> parseMetainfo(const BencodeValue&)` validating required keys,
types, that `pieces` length is a multiple of 20, and that piece count is
consistent with total length and piece_length. Surface clear `Error`s for each
malformed case. Defer multi-file `files` handling to 0018. Build the `PieceLayout`
(task 0019) from the parsed fields.

## Acceptance Criteria

- Parses announce, name, piece_length, length, and per-piece hashes from a valid single-file torrent.
- Rejects missing/wrong-typed keys and `pieces` not divisible by 20 with specific errors.
- Derives correct piece count and last-piece size.
- Produces a populated `PieceLayout`.

## Testing Strategy

Unit tests with a small committed single-file `.torrent` fixture: assert parsed
fields and piece count. Negative tests: drop `piece length`, corrupt `pieces`
length, wrong types. Deterministic, offline, fixture in repo.

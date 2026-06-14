# Task 0045 — File selection model

**Milestone:** M9 — Selective downloads
**Estimated Effort:** 0.5 hours
**Depends on:** 0008

## Introduction (for project managers)

Users can choose to download only some files from a multi-file torrent (by index,
range, or name pattern), as the README shows. This task parses and represents
that selection request, independent of how it maps onto pieces.

## Detailed Design (for engineers)

Define `FileSelection` and a parser for CLI inputs: `--files 2,5,7`, ranges
`2-9`, and `--files-matching <glob>` against file paths. Produce a normalized set
of selected file indices, validated against the torrent's file count (reject
out-of-range). Support "all files" default. Pure domain logic.

## Acceptance Criteria

- Parses index lists, ranges, and glob patterns into a validated set of file indices.
- Rejects indices outside the torrent's file count with a clear error.
- Defaults to all files when no selection is given.

## Testing Strategy

Unit tests: parse `2,5,7`, `2-9`, mixed `1,3-5,8`, and globs against a fixture
file list; out-of-range and malformed inputs return errors. Deterministic.

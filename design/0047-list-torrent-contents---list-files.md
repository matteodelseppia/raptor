# Task 0047 — List torrent contents (--list-files)

**Milestone:** M9 — Selective downloads
**Estimated Effort:** 0.25 hours
**Depends on:** 0018

## Introduction (for project managers)

A simple but useful feature: print the files inside a torrent (names, sizes,
indices) without downloading anything, so users know what to select. The README
shows `--list-files`.

## Detailed Design (for engineers)

Implement a `listFiles(Metainfo)` formatter producing index, path, and
human-readable size per file (and total). Wire to a `--list-files` CLI path
(0053) that parses the torrent and prints the table, then exits without
networking.

## Acceptance Criteria

- Prints index, path, and size for each file plus a total, from a parsed torrent.
- `--list-files` exits cleanly without contacting any network.
- Works for both single- and multi-file torrents.

## Testing Strategy

Unit test the formatter against single/multi-file fixtures (assert exact text /
parsed rows). CLI path covered in end-to-end harness (0055). Deterministic.

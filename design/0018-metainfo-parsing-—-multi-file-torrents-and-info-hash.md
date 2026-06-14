# Task 0018 — Metainfo parsing — multi-file torrents and info hash

**Milestone:** M3 — Metainfo
**Estimated Effort:** 0.75 hours
**Depends on:** 0015, 0016, 0017

## Introduction (for project managers)

Many torrents bundle multiple files in a folder. This task handles that layout
and computes the torrent's unique fingerprint (the "info hash") that trackers and
peers use to identify the swarm. Getting the info hash byte-exact is essential —
peers reject a wrong one.

## Detailed Design (for engineers)

Extend the parser to handle `info.files` (list of `{length, path[]}`),
producing an ordered file list with byte offsets into the global piece space and
total length. Compute the **info hash** as `IHasher::sha1` over the *raw bytes of
the `info` dictionary exactly as they appeared in the source* — use the
byte-range capability from the decoder (0014) rather than re-encoding, to avoid
any canonicalization mismatch (also keep a re-encode path as a cross-check).
Validate path components (reject empty, `..`, absolute paths) for safety.

## Acceptance Criteria

- Parses multi-file `files` into an ordered list with correct global offsets and total length.
- Info hash is computed from the original raw `info` bytes and matches reference clients.
- Unsafe paths (`..`, absolute, empty) are rejected.
- Single-file path from 0017 still works.

## Testing Strategy

Unit tests with a committed multi-file `.torrent` fixture: assert file list,
offsets, total size, and that the computed info hash equals a known-good hex
value (precomputed with a reference tool and stored in the test). Path-traversal
negative tests. Deterministic, offline.

# Task 0011 — Filesystem interface and adapter

**Milestone:** M1 — Core abstractions
**Estimated Effort:** 0.75 hours
**Depends on:** 0008

## Introduction (for project managers)

Raptor reads torrent files and writes downloaded data to disk. We hide all disk
access behind an interface so we can test piece-writing logic with an in-memory
fake instead of touching the real filesystem, keeping tests fast and safe.

## Detailed Design (for engineers)

In `interfaces/`, define `class Raptor::FileSystem` with the operations Raptor needs:
`open`/`create` a file of a given size (sparse/preallocated),
`WriteAt(handle, offset, span<const byte>)`, `ReadAt(...)`, `Exists`, `Size`,
`Rename`, `Remove`, and directory creation for multi-file torrents. All return
`Status`/`Result` (task 0008). Provide a `PosixFileSystem`/`StdFileSystem`
adapter in infrastructure using `<filesystem>` + positional I/O, and an
`InMemoryFileSystem` fake in test-support storing buffers in a map. Ownership of
file handles is RAII (`FileHandle` move-only wrapper).

## Acceptance Criteria

- `FileSystem` covers create/read/write-at-offset/exists/size/rename/remove and lives in `interfaces/`.
- `StdFileSystem` performs real positional I/O; `InMemoryFileSystem` mirrors behaviour in memory.
- File handles are RAII and move-only.
- Errors are reported via `Result`/`Status`, not exceptions, for expected failures.

## Testing Strategy

Unit-test the `InMemoryFileSystem` against a shared behavioural test suite
(write-at-offset then read-back, size growth, missing-file errors). Run the same
parametrized suite against `StdFileSystem` using a temp directory to prove
parity. Deterministic and offline.

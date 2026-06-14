# Task 0048 — Resume state format and persistence

**Milestone:** M10 — Resume
**Estimated Effort:** 0.5 hours
**Depends on:** 0011, 0015, 0030

## Introduction (for project managers)

Downloads shouldn't restart from zero if interrupted. This task defines a small
on-disk record of progress (which pieces are verified) so Raptor can pick up where
it left off — the README's "resume support".

## Detailed Design (for engineers)

Define a resume file (bencoded via 0015, or a simple binary) stored next to the
output: info_hash (to detect mismatch), piece count, the verified-pieces bitfield
(0030), total downloaded, and a format version. Implement `saveResume(Storage
location, ResumeState)` via `IFileSystem` written atomically (temp + rename).
Persist periodically and on clean shutdown.

## Acceptance Criteria

- Resume file records info_hash, version, and the verified-piece bitfield.
- Written atomically (temp file + rename) to avoid corruption on crash.
- Round-trips: saved state decodes back to an equal `ResumeState`.

## Testing Strategy

Unit tests with `InMemoryFileSystem`: save -> load round-trip equality; atomic
write leaves no partial file on simulated failure mid-write; version field
present. Deterministic.

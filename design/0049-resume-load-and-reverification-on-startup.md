# Task 0049 — Resume load and reverification on startup

**Milestone:** M10 — Resume
**Estimated Effort:** 0.75 hours
**Depends on:** 0033, 0034, 0048

## Introduction (for project managers)

On restart, Raptor reads the saved progress and confirms the data on disk is
actually intact before trusting it, then continues downloading only what's
missing. This makes resume both fast and safe.

## Detailed Design (for engineers)

Implement startup resume: if a resume file exists and its info_hash matches, load
the verified bitfield; optionally (config `--verify`) re-read each
supposedly-complete piece from `Storage` (0034) and re-hash (0033) to confirm,
clearing any that fail. Initialize the picker's needed-set to the unverified
pieces. On info_hash mismatch or missing file, start fresh. Handle the case where
output files are missing/short (treat as incomplete).

## Acceptance Criteria

- Loads and trusts a matching resume file; mismatched info_hash starts fresh.
- Optional reverification re-hashes complete pieces and drops any that fail.
- Picker resumes requesting only the genuinely missing pieces.
- Missing/short output files are handled as incomplete, not crashes.

## Testing Strategy

Unit tests: resume with valid state -> only missing pieces requested; corrupt one
on-disk piece -> reverification clears it and it is re-requested; mismatched
info_hash -> full restart; truncated file -> treated incomplete. Uses
`InMemoryFileSystem`. Deterministic.

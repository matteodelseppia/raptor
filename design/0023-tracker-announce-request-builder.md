# Task 0023 — Tracker announce request builder

**Milestone:** M5 — Tracker
**Estimated Effort:** 0.5 hours
**Depends on:** 0018, 0022

## Introduction (for project managers)

To find peers, Raptor asks a "tracker" server, sending it the torrent id and our
download progress. This task builds that request correctly — the tracker ignores
or rejects malformed requests, so the details matter.

## Detailed Design (for engineers)

Define `AnnounceRequest` carrying `info_hash` (Hash20), `peer_id` (20 bytes,
generated with an Azureus-style prefix e.g. `-RA0001-` + random), `port`,
`uploaded`, `downloaded`, `left`, `compact=1`, `event`
(`started/stopped/completed/empty`), and optional `numwant`. Implement
`Url buildAnnounceUrl(base, AnnounceRequest)` using the HTTP client's
percent-encoding (0022). Generate and persist a stable per-session `peer_id`.

## Acceptance Criteria

- Builds a correct announce URL with all required parameters and exact binary encoding.
- `peer_id` is 20 bytes with a recognizable Raptor prefix and stable per session.
- `event` values map to correct strings; omitted when empty.

## Testing Strategy

Unit tests: assert the constructed query string matches expected encoding for a
known info_hash/peer_id; event mapping; numwant inclusion/omission. Deterministic
(seed the peer_id RNG in tests).

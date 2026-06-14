# Task 0024 — Tracker response parser

**Milestone:** M5 — Tracker
**Estimated Effort:** 0.5 hours
**Depends on:** 0014, 0023

## Introduction (for project managers)

The tracker replies with a list of peers and how long to wait before asking
again. This task decodes that reply, handling both the compact binary peer format
and the older dictionary format, and surfaces tracker error messages.

## Detailed Design (for engineers)

Implement `Result<AnnounceResponse> parseAnnounce(span<const byte> body)`:
bdecode the body, handle `failure reason` (return `Error`), read `interval`
(and optional `min interval`), and parse peers in **compact** form (6 bytes:
4-byte IPv4 + 2-byte port, repeated) and **non-compact** dict-list form. Produce
`std::vector<Endpoint>` plus interval. Tolerate and skip malformed peer entries
defensively. (IPv6 compact `peers6` optional, can be a follow-up.)

## Acceptance Criteria

- Parses compact (6-byte) peer lists and dictionary peer lists into endpoints.
- Reads `interval`/`min interval`.
- `failure reason` responses become `Error` with the tracker's message.
- Malformed/truncated peer blobs are handled without crashing.

## Testing Strategy

Unit tests with canned bencoded responses (compact and dict forms), a
failure-reason response, and a truncated compact list. Assert endpoints and
interval. Deterministic, offline.

# Task 0016 — SHA-1 hashing interface and adapter

**Milestone:** M3 — Metainfo
**Estimated Effort:** 0.5 hours
**Depends on:** 0008, 0012

## Introduction (for project managers)

BitTorrent identifies torrents and verifies downloaded data using SHA-1 hashes.
We wrap hashing behind an interface (consistent with hiding third-party tools) so
the algorithm implementation can be swapped and so tests can inject a predictable
hasher.

## Detailed Design (for engineers)

In `interfaces/`, define `class IHasher` with streaming API
(`reset`, `update(span<const byte>)`, `Hash20 finalize()`) and a one-shot
`Hash20 sha1(span<const byte>)`. Provide an implementation in infrastructure —
either a small vetted public-domain SHA-1 or a crypto lib fetched via
FetchContent (prefer a tiny header-only SHA-1 to avoid heavy deps; document
choice). SHA-1 is required by the protocol (info hash + per-piece digests); note
it is used for protocol compatibility, not security. Keep implementation behind
the interface.

## Acceptance Criteria

- `IHasher` provides streaming + one-shot SHA-1 producing a `Hash20`.
- Output matches SHA-1 test vectors exactly.
- Algorithm implementation is confined to infrastructure.

## Testing Strategy

Unit tests against published SHA-1 vectors (empty string, "abc", long input).
Streaming-vs-one-shot equivalence test. A `FakeHasher` (returns a deterministic
digest) supports higher-level tests. Deterministic.

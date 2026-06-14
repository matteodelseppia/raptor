# Task 0012 — Byte buffer and span utilities

**Milestone:** M1 — Core abstractions
**Estimated Effort:** 0.5 hours
**Depends on:** 0008

## Introduction (for project managers)

The BitTorrent protocol is binary: integers packed big-endian, fixed-size hash
blobs, length-prefixed messages. We build small, well-tested helpers for reading
and writing these byte sequences so the rest of the code stays clean and free of
error-prone pointer math.

## Detailed Design (for engineers)

In `domain/` add a header-only `ByteReader`/`ByteWriter` over
`std::span<const std::byte>` / growable `std::vector<std::byte>`:
bounds-checked `readU8/U16/U32/U64` (big-endian, network order), `readBytes(n)`,
`writeUxx`, `writeBytes`. Reads return `Result` on overrun (no UB). Add a
fixed-size `Hash20` (20-byte SHA-1 digest) value type with hex formatting and
hashing for use in maps. Use `std::string_view`/`std::span` at call sites; avoid
copies. No third-party deps.

## Acceptance Criteria

- Big-endian read/write round-trips for 8/16/32/64-bit values.
- Out-of-bounds reads return an `Error`, never crash.
- `Hash20` supports equality, hashing, and hex formatting.
- Header is third-party-free and `constexpr`-friendly where possible.

## Testing Strategy

Unit tests: round-trip each integer width; assert big-endian byte order against
known vectors; assert overrun returns error; `Hash20` hex round-trip. Edge
cases: empty span, exact-boundary reads. Deterministic.

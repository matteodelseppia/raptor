# Task 0015 — Bencode encoder

**Milestone:** M2 — Bencode
**Estimated Effort:** 0.5 hours
**Depends on:** 0013, 0014

## Introduction (for project managers)

The reverse of the previous task: turning structured values back into bencode
bytes. We need this to build tracker requests and to save resume data. It must
produce the exact canonical bytes the protocol expects.

## Detailed Design (for engineers)

Implement `std::vector<std::byte> encode(const BencodeValue&)`. Dictionaries
must be emitted with keys sorted lexicographically by raw bytes (BitTorrent
canonical form) — essential so that re-encoding yields identical bytes (and
identical info hashes). Integers without leading zeros; byte strings with correct
length prefix. Provide a `decode(encode(v)) == v` round-trip guarantee for
canonical inputs.

## Acceptance Criteria

- Produces canonical bencode (dict keys sorted by raw byte order).
- `encode` then `decode` round-trips to an equal value.
- Byte-exact output matches reference vectors.

## Testing Strategy

Round-trip property tests (decode->encode->decode) over fixtures. Byte-exact
comparison against known canonical encodings. Verify key reordering of an
unsorted input produces sorted output. Deterministic.

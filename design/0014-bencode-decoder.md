# Task 0014 — Bencode decoder

**Milestone:** M2 — Bencode
**Estimated Effort:** 1 hour
**Depends on:** 0013

## Introduction (for project managers)

This task teaches Raptor to read the bencode format — turning the raw bytes of a
.torrent file or tracker response into the structured values from the previous
task. Correctness here is critical: a wrong parse means the wrong file gets
downloaded.

## Detailed Design (for engineers)

Implement `Result<BencodeValue> decode(std::span<const std::byte>)` plus a
variant that also returns the number of bytes consumed (needed to locate the raw
`info` dict for hashing, task 0018). Handle: integers `i<n>e` (reject leading
zeros, `-0`, empty), byte strings `<len>:<bytes>` (bounds-checked length),
lists `l...e`, dicts `d...e` (keys must be byte strings, and the decoder should
verify keys are in ascending order or at least record raw key spans). Strictly
reject trailing garbage, truncation, and malformed length prefixes — all via
`Error` (no exceptions for malformed input). Guard recursion depth to avoid
stack overflow on adversarial input.

## Acceptance Criteria

- Decodes all four types and arbitrarily nested structures.
- Rejects malformed input (leading zeros, bad lengths, truncation, trailing bytes) with `Error`.
- Recursion depth is bounded; deeply nested input fails gracefully, never crashes.
- Can report the byte range of any sub-value (for info-dict hashing).

## Testing Strategy

Extensive unit tests with known vectors (e.g. `d3:cow3:moo4:spam4:eggse`).
Property/edge tests: empty input, truncated strings, oversized length,
non-string dict keys, integer overflow, deeply nested input (depth guard). A
small real `.torrent` fixture decodes successfully. Fuzz-style table of malformed
inputs all return errors. Deterministic, offline.

# Task 0013 — Bencode value model

**Milestone:** M2 — Bencode
**Estimated Effort:** 0.5 hours
**Depends on:** 0008, 0012

## Introduction (for project managers)

Torrent files and tracker replies are written in a simple format called
"bencode". Before we can read or write that format we need an in-memory
representation of its four data types. This task defines that data structure.

## Detailed Design (for engineers)

In `domain/`, define `BencodeValue` as a variant of the four bencode types:
`std::int64_t`, byte string (`std::vector<std::byte>` / accessible as
`string_view`), list (`std::vector<BencodeValue>`), and dictionary (an
**ordered** `std::vector<std::pair<key,value>>` or a `std::map` — keys must be
sortable for canonical re-encoding). Provide typed accessors returning `Result`
(`asInt`, `asString`, `asList`, `asDict`, `at(key)`). Keep it a pure value type
(regular, comparable). This underpins both the decoder (0014) and encoder (0015).

## Acceptance Criteria

- `BencodeValue` represents int, byte-string, list, and dict.
- Dictionary preserves a canonical (sorted-by-key) ordering for re-encoding.
- Typed accessors return `Result` on type mismatch.
- Value type is copyable, movable, and equality-comparable.

## Testing Strategy

Unit tests: construct each variant, assert accessor type-checks (wrong-type
returns error), equality semantics, nested structures. Deterministic.

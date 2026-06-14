# Task 0026 — Peer message model and (de)serialization

**Milestone:** M6 — Peer protocol
**Estimated Effort:** 0.75 hours
**Depends on:** 0012

## Introduction (for project managers)

Once connected, peers exchange a small set of typed messages (e.g. "I have piece
5", "send me this block"). This task defines those message types and how they
convert to and from bytes — the vocabulary of all peer communication.

## Detailed Design (for engineers)

Define the peer message set as a `std::variant` (domain): `KeepAlive`, `Choke`,
`Unchoke`, `Interested`, `NotInterested`, `Have{index}`, `Bitfield{bytes}`,
`Request{index, begin, length}`, `Piece{index, begin, block}`,
`Cancel{index, begin, length}`, `Port{listen_port}`. Implement
`encode(msg) -> bytes` (4-byte big-endian length prefix + 1-byte id + payload;
keep-alive is a zero-length prefix) and `Result<PeerMessage> decodeMessage(span)`
using the byte utilities (0012). Validate ids and payload lengths.

## Acceptance Criteria

- All standard peer messages round-trip through encode/decode byte-exactly.
- Length-prefix framing (incl. zero-length keep-alive) is correct and big-endian.
- Unknown ids / wrong payload lengths return `Error`.

## Testing Strategy

Unit tests: encode each message and compare to reference byte vectors; decode
back to equal value. Negative tests: unknown id, truncated payload, length
mismatch. Deterministic.

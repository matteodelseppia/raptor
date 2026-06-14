# Task 0028 — Streaming message framer

**Milestone:** M6 — Peer protocol
**Estimated Effort:** 0.75 hours
**Depends on:** 0026

## Introduction (for project managers)

Data arrives from the network in arbitrary chunks, not neat messages. This task
reassembles the byte stream into complete peer messages, buffering partial data
until a whole message is available — a common source of bugs if done carelessly.

## Detailed Design (for engineers)

Implement `MessageFramer`: feed it incoming bytes (`push(span)`), it yields zero
or more complete `PeerMessage`s, retaining any partial tail for the next push.
Read the 4-byte length prefix, wait for the full frame, then decode (0026).
Enforce a maximum frame size (reject absurd lengths to prevent memory-blowup
attacks). Separately handle the initial handshake (fixed 68 bytes) before the
framed stream begins.

## Acceptance Criteria

- Correctly reassembles messages split across arbitrary byte boundaries.
- Coalesces multiple messages arriving in one chunk.
- Rejects frames exceeding a configured maximum size with an `Error`.
- Handles the fixed-size handshake phase distinctly from framed messages.

## Testing Strategy

Unit tests: feed a known multi-message stream one byte at a time, in random
chunk sizes, and all-at-once — assert identical decoded sequence each way.
Oversized-length-prefix rejection test. Deterministic, table-driven.

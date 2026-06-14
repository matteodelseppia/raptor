# Task 0027 — Handshake encode/decode

**Milestone:** M6 — Peer protocol
**Estimated Effort:** 0.5 hours
**Depends on:** 0012, 0018, 0023

## Introduction (for project managers)

Every peer conversation opens with a fixed "handshake" that proves both sides
want the same torrent. If it doesn't match exactly, the peer hangs up. This task
implements that opening exchange.

## Detailed Design (for engineers)

Implement the 68-byte handshake: 1-byte pstrlen (19), pstr
`"BitTorrent protocol"`, 8 reserved bytes (zero for now; bit flags reserved for
extensions), 20-byte info_hash, 20-byte peer_id. Provide `encodeHandshake` and
`Result<Handshake> decodeHandshake(span)` that validates pstrlen/pstr and returns
the remote info_hash + peer_id. Caller compares info_hash to the active torrent
and rejects mismatches.

## Acceptance Criteria

- Produces the exact 68-byte handshake.
- Decodes and validates pstrlen/pstr; returns remote info_hash and peer_id.
- Mismatched protocol string or short buffer returns `Error`.

## Testing Strategy

Unit tests: byte-exact handshake encoding; decode round-trip; reject wrong
pstr/length and truncated input. Deterministic.

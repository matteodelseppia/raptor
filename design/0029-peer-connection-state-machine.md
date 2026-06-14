# Task 0029 — Peer connection state machine

**Milestone:** M6 — Peer protocol
**Estimated Effort:** 1 hour
**Depends on:** 0021, 0027, 0028, 0030

## Introduction (for project managers)

This is the per-peer brain: it tracks whether we're allowed to request data,
whether we're interested, keeps the connection alive, and reacts to the peer's
messages. It turns a raw socket into a well-behaved BitTorrent peer relationship.

## Detailed Design (for engineers)

Implement `PeerConnection` owning a socket (0021), framer (0028), and the four
protocol state bits (`am_choking`, `am_interested`, `peer_choking`,
`peer_interested`, all starting choked/not-interested). Drive the lifecycle:
connect -> handshake (0027) -> exchange bitfield -> message loop. Handle inbound
`Have`/`Bitfield` (update availability via 0030), `Choke`/`Unchoke`,
`Request`/`Piece`/`Cancel` (delegate to scheduler/storage later). Send keep-alives
every ~2 min via `IClock`; drop peers idle past a timeout. Expose hooks/callbacks
for higher layers (block received, unchoked, disconnected). Errors close the
connection cleanly.

## Acceptance Criteria

- Completes connect -> handshake -> bitfield handshake and enters the message loop.
- Maintains the four choke/interest state bits per protocol rules.
- Sends keep-alives on schedule and times out idle peers (clock-driven).
- Surfaces inbound blocks/state changes via callbacks; cleans up on error/disconnect.

## Testing Strategy

Unit tests with `FakeTcpSocket` (scripted inbound bytes) + `FakeClock`: assert
state transitions on choke/unchoke/interested, keep-alive emission after advancing
the clock, idle timeout disconnect, and that a received `Piece` triggers the
block callback. Deterministic, no network.

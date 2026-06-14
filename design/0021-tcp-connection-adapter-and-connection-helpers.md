# Task 0021 — TCP connection adapter and connection helpers

**Milestone:** M4 — Network
**Estimated Effort:** 0.5 hours
**Depends on:** 0020

## Introduction (for project managers)

This task adds the convenience layer around raw sockets — establishing
connections to a peer's address with timeouts and clean teardown — so peer code
can say "connect to this peer" without dealing with low-level details.

## Detailed Design (for engineers)

Provide `Endpoint` (IP + port value type) and a `TcpConnector` built on
`IoRuntime`/`ITcpSocket`: `Result<TcpSocketPtr> connect(Endpoint, deadline)`
with connection timeout via the clock, plus framed read/write helpers
(`readExactly(n)`, `writeAll`). Ensure RAII socket ownership and idempotent
close. This is the substrate for the tracker HTTP client (0022) and peer
connections (0029).

## Acceptance Criteria

- `connect` honours a deadline and returns an `Error` on timeout/refusal.
- `readExactly`/`writeAll` loop until complete or error.
- Sockets are RAII; double-close is safe.

## Testing Strategy

Loopback tests: successful connect+echo, connect timeout to an unroutable
endpoint (using FakeClock-driven deadline or a closed port), partial-read
assembly via `readExactly`. `FakeTcpSocket` covers timeout/partial-read logic
deterministically.

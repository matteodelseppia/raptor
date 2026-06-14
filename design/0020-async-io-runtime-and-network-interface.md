# Task 0020 — Async I/O runtime and network interface

**Milestone:** M4 — Network
**Estimated Effort:** 1 hour
**Depends on:** 0003, 0008, 0010

## Introduction (for project managers)

Raptor talks to many peers at once. Doing that efficiently needs asynchronous
networking. This task chooses the engine (the asio library) and wraps it behind
Raptor interfaces, so the bulk of the code deals with simple abstractions and the
networking library stays replaceable.

## Detailed Design (for engineers)

In `interfaces/`, define the async surface Raptor needs without leaking asio:
`class IoRuntime` (run/stop/post, owns the event loop), `class ITcpSocket`
(async `connect`, `read`/`readSome`, `write`, `close`; completion via callbacks
or a lightweight awaitable). Decide the concurrency model now: prefer C++20
coroutines over asio for readability (document the choice). In infrastructure,
implement `AsioIoRuntime` and `AsioTcpSocket` wrapping standalone asio. Keep all
asio includes in infrastructure `.cpp`/private headers. Define cancellation and
timeout semantics in terms of `IClock`/deadlines.

## Acceptance Criteria

- `IoRuntime` and `ITcpSocket` abstractions contain no asio types in their headers.
- Coroutine/callback model is consistent and documented.
- `AsioIoRuntime` runs an event loop; `AsioTcpSocket` performs async connect/read/write.
- Timeouts/cancellation are expressible and tied to the clock abstraction.

## Testing Strategy

Loopback integration test (allowed: localhost only, not the internet) where an
asio server and client exchange bytes via `ITcpSocket`. A `FakeTcpSocket`
(scripted read/write sequences) enables deterministic unit tests of higher
layers without sockets. CLAUDE.md bans *real peers/trackers/internet*; localhost
loopback in a controlled test is the accepted substitute.

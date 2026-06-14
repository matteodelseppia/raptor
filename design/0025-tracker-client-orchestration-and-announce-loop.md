# Task 0025 — Tracker client orchestration and announce loop

**Milestone:** M5 — Tracker
**Estimated Effort:** 0.75 hours
**Depends on:** 0010, 0023, 0024

## Introduction (for project managers)

This ties the tracker pieces together: send the first announce, schedule the next
one at the interval the tracker specifies, retry on failure, and feed discovered
peers to the rest of Raptor. It is how the swarm is continuously discovered.

## Detailed Design (for engineers)

Implement `TrackerClient` depending on `IHttpClient`, `IClock`, `ILogger`. API:
`start(event=started)`, periodic `reannounce` driven by the tracker `interval`
(timer via `IClock`), `complete()` and `stop()` events, and `announce-list`
failover (try tiers/URLs in order). Emit discovered peers via a callback/queue to
the connection manager (0044). Apply exponential backoff on errors, capped.
Log announce results (peer count, interval) per the project's
"useful, structured, short" logging guidance.

## Acceptance Criteria

- Sends `started` on start, periodic announces at the tracker interval, `completed`/`stopped` at the right times.
- Falls over across `announce-list` URLs on failure.
- Applies capped exponential backoff on errors.
- Emits discovered peers to a consumer; logs concise announce summaries.

## Testing Strategy

Unit tests with `FakeHttpClient` (canned responses incl. failures) and
`FakeClock`: assert announce sent on start, re-announce after advancing past the
interval, backoff growth on consecutive failures, failover to the second URL.
Deterministic, no network.

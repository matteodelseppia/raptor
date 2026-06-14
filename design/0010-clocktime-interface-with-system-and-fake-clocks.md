# Task 0010 — Clock/time interface with system and fake clocks

**Milestone:** M1 — Core abstractions
**Estimated Effort:** 0.5 hours
**Depends on:** 0008

## Introduction (for project managers)

Lots of Raptor's logic depends on time — request timeouts, download-rate
measurement, tracker re-announce intervals. To test that logic quickly and
deterministically we must be able to fake the clock. This task introduces a time
abstraction so tests never wait for real seconds to pass.

## Detailed Design (for engineers)

In `interfaces/`, define `class Raptor::Clock { virtual time_point Now() const; }`
using `std::chrono::steady_clock::time_point` for monotonic durations (and a
separate `system_clock` accessor only where wall time is needed, e.g. logs).
Provide `SystemClock` in infrastructure and `FakeClock` (test double, advanceable
via `advance(duration)`) in a test-support library. Provide a `Timer`/deadline
helper built on `Raptor::Clock`. All rate/timeout code must depend on `Clock`, never
call `chrono::now()` directly.

## Acceptance Criteria

- `Clock` abstracts monotonic now(); production code never calls `steady_clock::now()` directly.
- `FakeClock` supports deterministic `advance()`.
- A deadline/timer helper computes elapsed/remaining from an injected clock.

## Testing Strategy

Unit tests drive `FakeClock`: advance time, assert deadline expiry and elapsed
calculations. `SystemClock` test asserts monotonic non-decreasing `now()`. Fully
deterministic.

# Task 0009 — Logging interface and spdlog adapter

**Milestone:** M1 — Core abstractions
**Estimated Effort:** 0.75 hours
**Depends on:** 0003, 0008

## Introduction (for project managers)

We want useful, structured logs about download speed and peer behaviour, but the
business logic should not be tied to a specific logging library. So we define
our own small logging interface; the actual library (spdlog) sits behind it and
can be swapped later. This follows the project rule that third-party tools stay
hidden.

## Detailed Design (for engineers)

In `interfaces/`, define `class Raptor::Logger` with leveled methods (`trace/debug/info/warn/error`) taking a `std::string_view` message and a `std::source_location` defaulted argument. Prefer a small structured-field overload (`info(msg, std::span<const Field>)` where `Field` is key/value) to support the structured logging the project wants. Provide a global accessor `Raptor::Log()` returning the active `Raptor::Logger&` plus an injection point for tests. In `infrastructure/`, implement `Raptor::Detail::SpdlogLogger : Raptor::Logger` wrapping spdlog sinks (console + rotating file). Enforce: **no `std::cout`/`std::cerr`** anywhere in production code; add a clang-tidy/grep CI guard if cheap. spdlog headers are included only in the adapter `.cpp`.

## Acceptance Criteria

- `Raptor::Logger` exposes leveled + structured logging and lives in `interfaces/` (no spdlog include).
- `Raptor::Detail::SpdlogLogger` routes to console and/or rotating file sinks.
- Production code references only `Raptor::Logger`; spdlog appears only in infrastructure details.
- No `std::cout`/`std::cerr` in `src/`.

## Testing Strategy

Use a `MockLogger` (GMock) to assert that components log expected events at
expected levels. Test `SpdlogLogger` writes to an in-memory/ostream sink and
formats levels correctly. Deterministic; no real files required (use temp dir).

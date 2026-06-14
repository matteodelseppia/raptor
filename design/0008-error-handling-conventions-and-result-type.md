# Task 0008 — Error handling conventions and Result type

**Milestone:** M1 — Core abstractions
**Estimated Effort:** 0.75 hours
**Depends on:** 0007

## Introduction (for project managers)

Software fails in predictable ways (a bad file, a refused connection). We decide
up front how the code reports such failures so every module behaves
consistently and callers always know what went wrong. This prevents silent bugs.

## Detailed Design (for engineers)

Define `Raptor::Error` — a small value type carrying an `enum class Errc`
(domain error categories: `InvalidBencode`, `InvalidMetainfo`, `Io`,
`Network`, `Protocol`, `HashMismatch`, `Cancelled`, ...) plus an optional
context `std::string`. Provide `using Result = std::expected<T, Error>` aliases
(`template<class T> using Result = std::expected<T, Error>;` and a `Status =
Result<void>`). Add helper makers (`Fail(Errc, std::string)`) and a
`std::error_code`/`std::error_category` mapping for interop. Document the policy
in a header comment: expected/recoverable failures return `Result`; truly
exceptional/programmer errors may throw. No exceptions cross interface
boundaries for expected failures. This lives in `domain/` (no third-party deps).

## Acceptance Criteria

- `Result<T>` and `Status` aliases compile on all toolchains (`std::expected` available in C++23).
- `Error` carries a category enum and optional message; equality works for tests.
- An `error_category` provides human-readable messages for each `Errc`.
- Header is third-party-free and lives under `domain/`.

## Testing Strategy

GTest unit tests: construct success/failure `Result`s, verify `has_value`,
`error().code()`, message propagation, and `and_then`/`transform` monadic
chaining. Verify `error_category` returns expected strings. Pure, deterministic.

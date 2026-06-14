# Task 0055 — End-to-end integration harness

**Milestone:** M12 — CLI & E2E
**Estimated Effort:** 1 hour
**Depends on:** 0050, 0054

## Introduction (for project managers)

Finally, a test that exercises the whole program like a real user would, but
entirely on the local machine with no internet — proving Raptor can download a
torrent end to end and that all the parts work together. This guards against
regressions as strategies are tweaked.

## Detailed Design (for engineers)

Build a self-contained integration harness (CTest) that: generates a small random
data set, creates a `.torrent` for it in-test, stands up a **loopback** minimal
tracker (returns the local seeder endpoint) and a minimal seeder speaking the real
peer protocol (0026–0029) serving the pieces, then runs Raptor (via `App::run`
with a `Config`) against `127.0.0.1`. Assert the downloaded output matches the
original bytes, resume works (kill mid-way, restart, completes), and
`--piece-order` variants all succeed. Keep it deterministic and fast; everything
stays on loopback to honour the "no real network/peers/trackers" rule.

## Acceptance Criteria

- Downloads a generated torrent end-to-end over loopback and reconstructs files byte-identically.
- Resume scenario: interrupt then restart completes the download.
- Each `--piece-order` mode completes successfully.
- Runs offline, deterministically, within CI time budget on all four toolchains.

## Testing Strategy

This task *is* the integration test. Mark it as a CTest test (possibly a separate
`integration` label so it can run after unit tests). Ensure it is hermetic
(loopback only, temp dirs, fixed seeds) so it passes identically across GCC 16,
Clang 22, AppleClang, and MSVC 19.

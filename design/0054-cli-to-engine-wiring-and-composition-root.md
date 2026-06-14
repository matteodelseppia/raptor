# Task 0054 — CLI to engine wiring and composition root

**Milestone:** M12 — CLI & E2E
**Estimated Effort:** 0.75 hours
**Depends on:** 0050, 0052, 0053

## Introduction (for project managers)

This task connects the parsed options to the real machinery: build the concrete
infrastructure adapters, construct the download session, run it, and report
results. It is the single place where everything is assembled — the program's
entry point.

## Detailed Design (for engineers)

Implement `main()` / `App::run(Config)` as the composition root: instantiate the
real adapters (`SpdlogLogger`, `SystemClock`, `StdFileSystem`, `AsioIoRuntime`,
`HttpClient`, `IHasher`), parse the torrent (0017/0018), resolve file selection
(0045/0046) or handle `--list-files` (0047), construct `DownloadSession` (0050)
with the chosen picker (0031 factory) and config, run the IO loop, handle
SIGINT for graceful shutdown (save resume), and return an exit code. No business
logic here — only wiring.

## Acceptance Criteria

- Builds all concrete adapters and a configured `DownloadSession` from `Config`.
- `--list-files` short-circuits to listing and exits.
- Ctrl-C triggers graceful shutdown that persists resume state.
- Returns meaningful exit codes; contains wiring only (no domain logic).

## Testing Strategy

Test via the end-to-end harness (0055) rather than unit tests (this is glue).
Where feasible, inject fakes into `App::run` to assert the correct wiring path is
chosen for `--list-files` vs download. Deterministic where mocked.

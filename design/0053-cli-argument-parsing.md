# Task 0053 — CLI argument parsing

**Milestone:** M12 — CLI & E2E
**Estimated Effort:** 0.75 hours
**Depends on:** 0003, 0045

## Introduction (for project managers)

The command-line interface is how users actually run Raptor. This task parses all
the options the README documents into a validated configuration object, with
helpful errors and `--help`.

## Detailed Design (for engineers)

Using CLI11 (0003) confined to the `cli/` layer, parse: positional `<torrent>`,
`--files`, `--files-matching`, `--list-files`, `--piece-order
optimized|sequential|reverse|random`, `--max-peers`, `--max-requests`,
`--no-seed`, `--output <dir>`, `--verify`, `--port`, verbosity flags, and
`--version`/`--help`. Map to a validated `Config` struct (domain) with sane
defaults from the README. Reject invalid combinations (e.g. `--files` with a
single-file torrent) with clear messages and non-zero exit. CLI11 types must not
leak past `cli/`.

## Acceptance Criteria

- Parses every README-documented flag into a validated `Config`.
- Invalid values/combinations produce clear errors and non-zero exit.
- `--help`/`--version` work; defaults match the README.
- CLI11 is confined to the `cli/` layer.

## Testing Strategy

Unit tests: parse representative argv arrays into `Config`, assert field values
and defaults; assert errors for bad `--piece-order`, out-of-range numbers, and
conflicting flags. Deterministic (parse from in-memory argv).

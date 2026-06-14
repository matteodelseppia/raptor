# Raptor — Design Task Index

This folder is the **task cave** from `CLAUDE.md`. Every code change comes from a
task file here. Tasks are executed **in numerical order**; a task may only start
once the tasks it depends on are *done* (code + tests + acceptance criteria +
Doxygen + formatting/lint all green).

Each task file contains: an **Introduction** (plain-language, for project
managers), a **Detailed Design** (for C++ engineers without BitTorrent
background), an **Estimated Effort** (≤ 1 hour), **Acceptance Criteria**, and a
**Testing Strategy**.

> **CI from day one.** Milestone M0 stands up GitHub Actions before any feature
> work, so tasks 0008 onward are covered by build + test + `clang-format` +
> `clang-tidy` checks on **Linux GCC 16**, **Linux Clang 22**, **macOS Apple
> Silicon (AppleClang)**, and **Windows MSVC 19**. Builds use **CMake presets**;
> dependencies are fetched with **FetchContent**.

## CMake Presets reference

`CMakePresets.json` (schema version 6) defines the following named presets.
All configure presets set `CMAKE_EXPORT_COMPILE_COMMANDS=ON` and use Ninja with
`build/<presetName>` as the binary directory.

| Preset name | Toolchain | Build type | Notes |
|---|---|---|---|
| `gcc-debug` | GCC 16 (`gcc-16` / `g++-16`) | Debug | Linux |
| `gcc-release` | GCC 16 | Release | Linux |
| `clang-debug` | Clang 22 (`clang-22` / `clang++-22`) | Debug | Linux |
| `clang-release` | Clang 22 | Release | Linux |
| `appleclang-debug` | Apple Clang (Xcode default) | Debug | macOS only |
| `appleclang-release` | Apple Clang | Release | macOS only |
| `msvc-debug` | MSVC (`cl`) | Debug | Windows only |
| `msvc-release` | MSVC | Release | Windows only |
| `dev` | Apple Clang (inherits `appleclang-debug`) | Debug | Enables `RAPTOR_SANITIZE=ON` (ASan + UBSan) |

Each configure preset has a matching **build preset** and **test preset**
(with `outputOnFailure: true`).

## How to build (after M0)

```bash
cmake --preset gcc-debug      # or clang-debug / appleclang-debug / msvc-debug
cmake --build --preset gcc-debug
ctest --preset gcc-debug --output-on-failure
cmake --build build/gcc-debug --target format-check   # clang-format gate
cmake --build build/gcc-debug --target tidy            # clang-tidy gate
```

## Milestones

| # | Milestone | Tasks | Outcome |
|---|-----------|-------|---------|
| M0 | Bootstrap & CI | 0001–0007 | Buildable skeleton; CI green on all 4 toolchains with format + tidy gates |
| M1 | Core abstractions | 0008–0012 | Error/Result, logging, clock, filesystem, byte utils (all behind interfaces) |
| M2 | Bencode | 0013–0015 | Decode/encode the torrent/tracker wire format |
| M3 | Metainfo | 0016–0019 | Parse `.torrent`, compute info hash, model piece layout |
| M4 | Network | 0020–0022 | Async runtime, TCP, HTTP client (all hidden behind interfaces) |
| M5 | Tracker | 0023–0025 | Discover peers via announce loop |
| M6 | Peer protocol | 0026–0030 | Handshake, framing, per-peer state machine, availability |
| M7 | Scheduling | 0031–0040 | Pluggable piece pickers + storage/verification + endgame |
| M8 | Peer strategy | 0041–0044 | Metrics, scoring, upload-ROI choking, connection management |
| M9 | Selective downloads | 0045–0047 | `--files`, ranges, patterns, `--list-files` |
| M10 | Resume | 0048–0049 | Persist + reverify progress across restarts |
| M11 | Session | 0050–0052 | Working download engine, stats, `--no-seed` completion |
| M12 | CLI & E2E | 0053–0055 | Full CLI + loopback end-to-end integration test |

## Task list

| Task | Title | Effort | Depends on |
|------|-------|--------|------------|
| 0001 | CMake project skeleton & directory layout | 0.75h | — |
| 0002 | CMake configure/build/test presets per toolchain | 0.75h | 0001 |
| 0003 | Dependency management via FetchContent | 1h | 0001, 0002 |
| 0004 | clang-format config and format-check target | 0.5h | 0001 |
| 0005 | clang-tidy configuration and lint check | 0.75h | 0002, 0003 |
| 0006 | GitHub Actions CI matrix across all toolchains | 1h | 0002–0005, 0007 |
| 0007 | First executable, CTest integration and smoke test | 0.5h | 0001–0003 |
| 0008 | Error handling conventions and Result type | 0.75h | 0007 |
| 0009 | Logging interface and spdlog adapter | 0.75h | 0003, 0008 |
| 0010 | Clock/time interface with system and fake clocks | 0.5h | 0008 |
| 0011 | Filesystem interface and adapter | 0.75h | 0008 |
| 0012 | Byte buffer and span utilities | 0.5h | 0008 |
| 0013 | Bencode value model | 0.5h | 0008, 0012 |
| 0014 | Bencode decoder | 1h | 0013 |
| 0015 | Bencode encoder | 0.5h | 0013, 0014 |
| 0016 | SHA-1 hashing interface and adapter | 0.5h | 0008, 0012 |
| 0017 | Metainfo parsing — single-file torrents | 0.75h | 0014, 0019 |
| 0018 | Metainfo parsing — multi-file torrents and info hash | 0.75h | 0015–0017 |
| 0019 | Piece layout model | 0.5h | 0012 |
| 0020 | Async I/O runtime and network interface | 1h | 0003, 0008, 0010 |
| 0021 | TCP connection adapter and connection helpers | 0.5h | 0020 |
| 0022 | HTTP client interface for tracker announces | 0.75h | 0020, 0021 |
| 0023 | Tracker announce request builder | 0.5h | 0018, 0022 |
| 0024 | Tracker response parser | 0.5h | 0014, 0023 |
| 0025 | Tracker client orchestration and announce loop | 0.75h | 0010, 0023, 0024 |
| 0026 | Peer message model and (de)serialization | 0.75h | 0012 |
| 0027 | Handshake encode/decode | 0.5h | 0012, 0018, 0023 |
| 0028 | Streaming message framer | 0.75h | 0026 |
| 0029 | Peer connection state machine | 1h | 0021, 0027, 0028, 0030 |
| 0030 | Bitfield and availability tracking | 0.5h | 0012, 0019 |
| 0031 | Piece picker interface (strategy abstraction) | 0.75h | 0019, 0030 |
| 0032 | Block request tracking (in-flight, timeouts) | 0.75h | 0010, 0019, 0031 |
| 0033 | Piece assembly and hash verification | 0.5h | 0016, 0019, 0032 |
| 0034 | Storage and file writer (single + multi-file) | 0.75h | 0011, 0019, 0033 |
| 0035 | Sequential scheduler | 0.5h | 0031 |
| 0036 | Reverse-sequential scheduler | 0.25h | 0031, 0035 |
| 0037 | Random scheduler | 0.5h | 0031 |
| 0038 | Rarest-first / availability-aware scheduler | 0.75h | 0030, 0031 |
| 0039 | Optimized default scheduler | 1h | 0032, 0038, 0040 |
| 0040 | Endgame mode | 0.5h | 0032, 0033 |
| 0041 | Peer performance metrics | 0.5h | 0010, 0029 |
| 0042 | Peer scoring and ranking | 0.5h | 0030, 0041 |
| 0043 | Choke/unchoke and upload-ROI controller | 1h | 0010, 0042 |
| 0044 | Connection manager (max-peers, churn) | 0.75h | 0025, 0029, 0042, 0043 |
| 0045 | File selection model | 0.5h | 0008 |
| 0046 | Map selected files to required pieces | 0.5h | 0019, 0045 |
| 0047 | List torrent contents (--list-files) | 0.25h | 0018 |
| 0048 | Resume state format and persistence | 0.5h | 0011, 0015, 0030 |
| 0049 | Resume load and reverification on startup | 0.75h | 0033, 0034, 0048 |
| 0050 | Download engine / session loop | 1h | 0025, 0031–0034, 0044, 0049 |
| 0051 | Progress and performance statistics reporting | 0.5h | 0010, 0041, 0050 |
| 0052 | Completion handling and --no-seed disconnect | 0.5h | 0043, 0050 |
| 0053 | CLI argument parsing | 0.75h | 0003, 0045 |
| 0054 | CLI to engine wiring and composition root | 0.75h | 0050, 0052, 0053 |
| 0055 | End-to-end integration harness | 1h | 0050, 0054 |

## Branch protection (required status checks)

After task 0006, configure the following required status checks on `main` (or
your default branch) via **Settings → Branches → Branch protection rules**:

| Required check | Job name in `ci.yml` |
|---|---|
| `build / linux-gcc16` | `build (linux-gcc16)` |
| `build / linux-clang22` | `build (linux-clang22)` |
| `build / macos-arm64` | `build (macos-arm64)` |
| `build / windows-msvc` | `build (windows-msvc)` |
| `lint / format-check` | `format-check` |
| `lint / tidy` | `tidy` |

All six must be green before a PR can merge.

## Conventions enforced by every task

- **Layering:** `domain/` never includes third-party headers. Libraries (spdlog,
  asio, CLI11, googletest) live only behind interfaces in `infrastructure/`.
- **Errors:** expected failures return `std::expected`-based `Result<T>`; no
  silent failures.
- **Testing:** GTest/GMock, deterministic, offline. Network is exercised only via
  fakes or **loopback** (never real peers/trackers/DHT/internet).
- **Done means done:** code + passing tests + Doxygen + format + tidy + layering
  respected, or the task is not done.

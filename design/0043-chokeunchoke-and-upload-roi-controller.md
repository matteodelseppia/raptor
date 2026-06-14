# Task 0043 — Choke/unchoke and upload-ROI controller

**Milestone:** M8 — Peer strategy
**Estimated Effort:** 1 hour
**Depends on:** 0010, 0042

## Introduction (for project managers)

This task implements Raptor's distinctive philosophy: upload only when it buys
faster downloads. It decides which peers to upload to (unchoke) based on whether
that upload is "earning" download bandwidth, and stops uploading when it doesn't
pay off — exactly the upload-as-a-tool stance in CLAUDE.md and the README.

## Detailed Design (for engineers)

Implement `ChokeController` running on a periodic tick (clock-driven, ~10s
standard interval, with an "optimistic unchoke" ~30s): rank peers by download
contribution (0042) and unchoke the top-N from whom we are *currently getting the
most download*; reserve one optimistic-unchoke slot to discover new fast peers.
Add **upload-ROI accounting**: track download bytes gained per upload byte spent
per peer; if a peer yields no download return over a window, choke it rather than
spend upload. Make N (`--max-peers`-related) and intervals configurable. Document
the ROI heuristic (intent/trade-offs) in Doxygen. This is the mechanism behind
"upload only when it speeds up download" and `--no-seed` post-completion (0052).

## Acceptance Criteria

- Periodically (clock-driven) unchokes the top contributors plus one optimistic slot.
- Measures download-gained-per-upload-spent per peer and chokes negative-ROI peers.
- Honours a configurable unchoke slot count and intervals.
- Provides the basis for stopping uploads when they don't aid download.

## Testing Strategy

Unit tests with `FakeClock` + scripted `PeerMetrics`: assert top contributors
unchoked each interval, optimistic slot rotates, a peer giving upload-but-no-
download gets choked after the ROI window, and the set updates as rates change.
Deterministic, no network.

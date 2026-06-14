# Task 0044 — Connection manager (max-peers, churn)

**Milestone:** M8 — Peer strategy
**Estimated Effort:** 0.75 hours
**Depends on:** 0025, 0029, 0042, 0043

## Introduction (for project managers)

This component manages the pool of peer connections: opening new ones from
tracker-discovered addresses up to a limit, and dropping the least useful peers to
make room for better ones. It keeps Raptor connected to the most productive
swarm members.

## Detailed Design (for engineers)

Implement `ConnectionManager` consuming peer endpoints from the tracker (0025),
opening `PeerConnection`s (0029) up to `--max-peers`, deduplicating addresses,
and maintaining a connect backoff for failed/disconnected peers. Periodically
prune the lowest-scored peers (0042) when at capacity and a better candidate is
waiting (churn), respecting a minimum dwell time to avoid thrashing. Wire choke
decisions from 0043. Surface connected-peer count/stats to 0051.

## Acceptance Criteria

- Maintains up to `--max-peers` connections; dedupes endpoints; backs off failed peers.
- Replaces lowest-scored peers with better candidates when at capacity (with anti-thrash dwell time).
- Integrates choke controller and feeds peer stats.

## Testing Strategy

Unit tests with fake connections/clock: fill to capacity, assert no over-connect;
feed a better candidate, assert worst peer pruned after dwell time; failed-connect
backoff. Deterministic.

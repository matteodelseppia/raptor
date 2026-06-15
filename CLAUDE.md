# CLAUDE.md

## Big Picture

**Raptor** = C++23 BitTorrent tool.

Raptor selfish.

Main goal:

* Download finish fast.
* Max throughput.
* Good peer use.
* Good piece grabbing.
* Fast completion.

Upload not goal.

Upload only tool.

If upload make download faster:

* Upload okay.

If upload not help download:

* No upload.

No long seeding.
No ratio chasing.
No fairness goals.

Code must allow easy testing and trying new download ideas.

---

# Main Law

**Download finish fast.**

When many choices, pick one that improves:

* Throughput
* Piece acquisition
* Peer utilization
* Connection effectiveness
* Completion time

Question always:

> Download finish faster?

If yes, good.

If no, probably bad.

---

## Upload Law

Upload bandwidth resource.

Spend upload only when return bigger download benefit.

Good:

* Stay unchoked.
* Gain faster peers.
* Gain more bandwidth.
* Improve completion time.

Bad:

* Upload with no return.
* Long-term seeding.
* Upload for fairness only.
* Upload for ratio only.

Measure return.

Do not assume.

---

# Structure

## Hide Outside Stuff

All outside tools hide behind Raptor interfaces.

Examples:

* Logging
* Network
* Files
* Time
* Torrent parsing
* Tracker talk
* Peer talk

Most code know interface only.

Most code never know third-party library.

Easy swap later.

---

## Dependency Path

```text
Application
    ↓
Domain
    ↓
Interfaces
    ↓
Infrastructure
    ↓
Third Party
```

Domain never depend on third-party.

---

# Logging

## Use

* spdlog

For:

* Console logs
* File logs

## Rules

No:

* std::cout logging
* std::cerr logging

Use logging abstraction.

spdlog stay inside infrastructure.

Business logic know interface only.

## Good Logs

Logs be:

* Useful
* Structured
* Short

Prefer logs about:

* Download speed
* Peer performance
* Piece scheduling
* Upload ROI
* Completion estimates

Avoid:

* Debug spam
* Repeat messages
* Low-value implementation details

---

# Task Driven Work

Every code change come from task file.

## Task Cave

```text
/design
```

Each task markdown file.

---

## Task Must Have

### Description

Explain:

* What build
* Why build
* What not included

### Acceptance Criteria

Must be:

* Testable
* Clear
* Measurable
* Use .clang-tidy naming conventions

### Unit Test Plan

Explain:

* What test
* What mock
* Coverage goal
* Edge cases

### Effort

Example:

```text
Estimated Effort: 0.75 hours
```

---

## Task Size

Task small.

### Max Size

One task ≤

```text
1 hour
```

Includes:

* Coding
* Tests
* Review prep

### If Too Big

Split task.

Keep order.

Each task verifiable alone.

---

## Task Order

Do tasks by number.

No future task before needed task done.

For dependencies:

1. Finish dependency.
2. Check acceptance criteria.
3. Run tests.
4. Move next.

---

# Testing

## Use

* GTest
* GMock

No other framework without approval.

---

## Unit Test Rules

Production code need tests.

Tests check:

* Behavior
* Edge cases
* Errors

Tests must:

* Deterministic
* Fast

Prefer behavior testing.

---

## Mocking

Use GMock.

Mock:

* Network
* Files
* Tracker
* Peer
* Logging
* Time

Do not mock simple value objects.

---

## Torrent Fixtures

Real torrent files okay for:

* Metadata parsing
* Info hash checks
* Piece layout checks

Fixtures must be:

* Small
* In repo
* Deterministic

---

## No Real Network

Tests never talk to:

* Real BitTorrent clients
* Public trackers
* Public DHT
* Real peers
* Internet services

Use:

* Mocks
* Fakes
* Local doubles

Tests run offline.

---

# Code Quality

## Modern C++

Prefer:

* std::expected
* std::optional
* std::span
* std::string_view
* Concepts
* Ranges

Avoid old style when modern style clearer.

---

## File Naming

All `.hpp` and `.cpp` filenames must use TitleCase (UpperCamelCase).

Examples:

* `PeerManager.hpp`
* `TorrentFile.cpp`
* `SpdlogLogger.cpp`
* `StdFileSystem.hpp`

Split on underscores and hyphens, capitalize each word, join without separator.

---

## Errors

Prefer explicit errors.

Use:

* std::expected
* Strong result types

Avoid:

* Silent fail
* Hidden error

Exceptions okay.

Expected failures usually explicit.

---

## Ownership

Ownership clear.

Prefer:

* RAII
* Smart pointers
* Value semantics

Avoid mystery ownership.

---

## Maintainability

Code be:

* Readable
* Testable
* Modular
* Replaceable

Clarity > cleverness.

---

# Scheduling

Piece scheduler core system.

Must allow easy experimentation.

Support ideas like:

* Optimized mode
* Sequential mode
* Reverse mode
* Random mode
* Availability-aware mode
* Peer-aware mode

New scheduler easy add.

Scheduler performance measurable.

---

# Peer Strategy

Peers not equal.

Track things like:

* Download rate
* Upload cost
* Reliability
* Choke behavior
* Piece availability
* Completion impact

Prefer peers helping finish fastest.

Remove peers wasting resources.

---

# Documentation

## Doxygen Required

All production code need Doxygen docs.

Document:

* Classes
* Structs
* Interfaces
* Public methods
* Enums
* Important free functions

### Complex Stuff

For hard algorithms explain:

* Intent
* Constraints
* Trade-offs

Do not explain obvious code.

Explain **why**, not only **what**.

---

# Performance

Performance feature.

Priority order:

1. Correctness
2. Testability
3. Download completion time
4. Maintainability

Measure before optimize.

Do not optimize too early.

Optimize critical paths:

* Piece scheduling
* Peer selection
* Upload ROI decisions
* Disk I/O
* Network I/O
* Message parsing

---

# Done Means Done

Task done only if:

* Code done
* Acceptance criteria pass
* Tests written
* Tests pass
* Doxygen present
* Logging rules followed
* Architecture respected
* No dependency leakage
* Formatting/static analysis pass
* Task doc still accurate

If one thing missing, task not done.

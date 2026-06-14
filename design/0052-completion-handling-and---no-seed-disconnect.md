# Task 0052 — Completion handling and --no-seed disconnect

**Milestone:** M11 — Session
**Estimated Effort:** 0.5 hours
**Depends on:** 0043, 0050

## Introduction (for project managers)

Raptor's whole philosophy is to stop when the job is done. This task implements
clean completion: send the tracker the "completed" event, optionally disconnect
immediately instead of seeding (`--no-seed`), and shut down tidily.

## Detailed Design (for engineers)

On completion (all needed pieces verified): send `completed` to the tracker
(0025), flush resume state (0048), print a final summary (0051). If `--no-seed`,
disconnect all peers and stop the session immediately. Otherwise, enter a minimal
seeding mode governed by the upload-ROI controller (0043) — i.e. upload only while
it might still help (consistent with the philosophy), with no fairness/ratio
goals. Provide an exit code reflecting success.

## Acceptance Criteria

- Sends `completed` to the tracker and writes final resume state on completion.
- `--no-seed` disconnects all peers and exits promptly with success.
- Default post-completion behaviour does not pursue ratio/fairness seeding.
- Prints a final completion summary.

## Testing Strategy

Unit/integration tests: drive a session to completion (fake swarm/clock); assert
`completed` announce sent, final summary emitted, and with `--no-seed` all peers
disconnected and the loop stops. Deterministic.

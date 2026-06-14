# Task 0022 — HTTP client interface for tracker announces

**Milestone:** M4 — Network
**Estimated Effort:** 0.75 hours
**Depends on:** 0020, 0021

## Introduction (for project managers)

The most common tracker type is contacted over plain HTTP. Raptor needs a minimal
HTTP GET capability for that. We keep it behind an interface so the tracker logic
doesn't care how the HTTP request is actually sent.

## Detailed Design (for engineers)

Define `class IHttpClient { Result<HttpResponse> get(Url, query params,
deadline); }` returning status code + raw body bytes. Implement a minimal HTTP/1.1
GET client over `TcpConnector` (request line, Host header, `Connection: close`,
chunked + content-length body handling) — sufficient for tracker announces; full
HTTP is out of scope. Provide URL parsing/encoding helpers (percent-encode the
binary info_hash and peer_id, which is subtle and must be exact). Keep an
extension point for a fetched HTTP lib if needed later. (UDP trackers are a
separate future task.)

## Acceptance Criteria

- `get` performs an HTTP/1.1 request and returns status + body for content-length and chunked responses.
- Binary query params (info_hash, peer_id) are percent-encoded byte-exactly.
- Deadlines are honoured.
- Interface contains no transport-specific types.

## Testing Strategy

Loopback test against a tiny in-test HTTP server returning fixed bodies (incl.
chunked). Unit tests for URL building/percent-encoding against known vectors
(the 20-byte info_hash encoding is the key case). `FakeHttpClient` returns canned
responses for tracker tests (0024/0025). Deterministic.

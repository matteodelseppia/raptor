# Raptor

Raptor is a modern C++23 BitTorrent command-line client built for one purpose:

> Finish downloads as quickly as possible.

Unlike traditional BitTorrent clients that attempt to balance downloading, uploading, fairness, and long-term swarm health, Raptor optimizes for a single metric:

> Download completion time.

---

# Why Raptor?

## Download-First Design

Raptor continuously optimizes for:

* Maximum download throughput
* Efficient piece acquisition
* Intelligent peer selection
* Fast completion time
* Minimal wasted bandwidth

Every major implementation decision is evaluated against a simple question:

> Does this help the download finish sooner?

---

## Strategic Uploading

Raptor is not a traditional seeding client.

Instead, uploads are treated as a tool for increasing download speed.

Raptor may upload data when doing so:

* Increases the likelihood of being unchoked
* Improves peer cooperation
* Unlocks additional download bandwidth
* Reduces overall completion time

Raptor avoids uploading when there is no measurable download benefit.

After download completion, Raptor can immediately disconnect instead of continuing to seed.

---

## Adaptive Peer Management

Not all peers are equally valuable.

Raptor continuously evaluates peers based on:

* Download rate
* Reliability
* Piece availability
* Responsiveness
* Contribution to overall completion time

Connections that do not improve performance can be deprioritized or removed.

---

## Flexible Download Strategies

Choose the strategy that best matches your goals.

Supported modes include:

* Performance optimized (default)
* Sequential piece download
* Reverse sequential download
* Random piece selection

This makes Raptor suitable for:

* High-speed downloads
* Streaming experiments
* Research
* Benchmarking
* Scheduler development

---

## Selective File Downloads

Multi-file torrents do not require downloading everything.

Raptor can:

* List torrent contents
* Download individual files
* Download file ranges
* Download files matching patterns

Only the required pieces are downloaded.

---

# Key Features

* Download completion time optimization
* Strategic upload-for-speed behavior
* Adaptive peer selection
* Selective file downloads
* Piece-order customization
* Resume support
* Hash verification
* Detailed performance statistics
* Lightweight command-line interface
* Modern C++23 implementation

---

# Examples

Download normally:

```bash
raptor ubuntu.torrent
```

Download only selected files:

```bash
raptor archive.torrent --files 2,5,7
```

List torrent contents:

```bash
raptor archive.torrent --list-files
```

Download sequentially:

```bash
raptor archive.torrent --piece-order sequential
```

Optimize aggressively for fastest completion:

```bash
raptor archive.torrent \
  --piece-order optimized \
  --max-peers 300 \
  --max-requests 256
```

Disable post-download seeding:

```bash
raptor archive.torrent --no-seed
```

---

# Philosophy

Traditional BitTorrent clients often optimize for:

* Fairness
* Ratio maintenance
* Long-term seeding
* Swarm contribution

Raptor optimizes for:

* Getting the data
* Getting it quickly
* Stopping when finished

If uploading 1 MB/s earns 20 MB/s of download bandwidth, Raptor will upload.

If uploading provides no download advantage, Raptor will stop.

The goal is not minimizing upload.

The goal is minimizing time-to-completion.

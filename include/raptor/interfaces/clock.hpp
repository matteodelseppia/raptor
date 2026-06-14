/**
 * @file clock.hpp
 * @brief Abstract monotonic clock interface for Raptor.
 *
 * All timeout, rate-measurement, and scheduling logic must depend on
 * this interface rather than calling
 * `std::chrono::steady_clock::now()` directly.  This makes time fully
 * controllable in tests via FakeClock.
 *
 * ## Usage (production)
 *
 * @code
 * void foo(const Raptor::Clock& clock) {
 *     auto t0 = clock.Now();
 *     // ... work ...
 *     auto elapsed = clock.Now() - t0;
 * }
 * @endcode
 *
 * ## Usage (tests)
 *
 * @code
 * FakeClock fake;
 * fake.Advance(std::chrono::seconds{5});
 * auto now = fake.Now(); // deterministic
 * @endcode
 */

#ifndef RAPTOR_INTERFACES_CLOCK_HPP
#define RAPTOR_INTERFACES_CLOCK_HPP

#include <chrono>

namespace Raptor {

// ---------------------------------------------------------------------------
// Common type aliases
// ---------------------------------------------------------------------------

/// Monotonic time point used throughout Raptor for durations and
/// deadlines.
using TimePoint = std::chrono::steady_clock::time_point;

/// Signed nanosecond-precision duration.
using Duration = std::chrono::steady_clock::duration;

// ---------------------------------------------------------------------------
// Clock — abstract interface
// ---------------------------------------------------------------------------

/**
 * @brief Abstract monotonic clock.
 *
 * Provides a single query — `Now()` — that returns the current
 * `TimePoint`.  Monotonicity is a contract; implementations must
 * never return a value earlier than a previously returned value.
 *
 * A separate `WallNow()` accessor is available for the concrete
 * `SystemClock` when human-readable wall-time is needed (e.g. log
 * timestamps).  Business logic should always use `Now()`.
 */
class Clock {
 public:
  virtual ~Clock() = default;

  Clock() = default;
  Clock(const Clock&) = delete;
  Clock& operator=(const Clock&) = delete;
  Clock(Clock&&) = delete;
  Clock& operator=(Clock&&) = delete;

  /**
   * @brief Returns the current monotonic time point.
   *
   * Guaranteed to be non-decreasing across successive calls.
   *
   * @return Current `TimePoint` from the underlying clock source.
   */
  [[nodiscard]] virtual TimePoint Now() const = 0;
};

}  // namespace Raptor

#endif  // RAPTOR_INTERFACES_CLOCK_HPP

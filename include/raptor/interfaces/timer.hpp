/**
 * @file timer.hpp
 * @brief Deadline / elapsed-time helper built on the Clock interface.
 *
 * `Timer` records a start point (via an injected `Clock`) and a fixed
 * timeout.  Callers can query elapsed time, remaining time, and
 * whether the deadline has been reached — all without touching
 * real-wall-clock APIs.
 *
 * ## Design intent
 *
 * Because `Timer` delegates all time queries to the injected `Clock`,
 * tests drive it deterministically through `FakeClock::Advance()`.
 * Production code passes a `SystemClock` reference; test code passes
 * a `FakeClock` reference.
 *
 * ## Usage
 *
 * @code
 * SystemClock clock;
 * Raptor::Timer t{clock, std::chrono::seconds{30}};
 *
 * // later...
 * if (t.IsExpired()) { // 30 s have elapsed
 *     reconnect();
 * }
 * auto remaining = t.Remaining(); // how much time is left
 * t.Reset();                      // restart from now
 * @endcode
 */

#ifndef RAPTOR_INTERFACES_TIMER_HPP
#define RAPTOR_INTERFACES_TIMER_HPP

#include "raptor/interfaces/Clock.hpp"

namespace Raptor {

/**
 * @brief Non-owning deadline timer built on an injected `Clock`.
 *
 * The timer captures `clock.Now()` at construction (and on
 * `Reset()`). All elapsed/remaining queries re-read `clock.Now()` at
 * call time so they reflect the passage of time since construction or
 * the last reset.
 *
 * @note The `Clock` reference must outlive the `Timer`.
 */
class Timer {
 public:
  /**
   * @brief Constructs a timer that expires after @p timeout.
   *
   * Captures the current time from @p clock as the start point.
   *
   * @param clock    Injected clock (must outlive this timer).
   * @param timeout  Duration until the deadline is reached.
   */
  Timer(const Clock& clock, Duration timeout) noexcept
      : mClock{clock}, mStart{clock.Now()}, mTimeout{timeout} {
  }

  // Non-copyable, non-movable — holds a reference.
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;
  Timer(Timer&&) = delete;
  Timer& operator=(Timer&&) = delete;

  /**
   * @brief Returns the duration elapsed since the timer was started
   * or last reset.
   *
   * @return Non-negative duration; zero if queried at the exact start
   * point.
   */
  [[nodiscard]] Duration Elapsed() const noexcept {
    return mClock.Now() - mStart;
  }

  /**
   * @brief Returns the duration remaining until the deadline.
   *
   * Returns `Duration::zero()` (not negative) once the deadline has
   * passed; callers do not need to clamp themselves.
   *
   * @return Remaining duration, clamped to zero.
   */
  [[nodiscard]] Duration Remaining() const noexcept {
    const auto elapsed = Elapsed();
    if (elapsed >= mTimeout) {
      return Duration::zero();
    }
    return mTimeout - elapsed;
  }

  /**
   * @brief Returns `true` if the deadline has been reached or
   * exceeded.
   */
  [[nodiscard]] bool IsExpired() const noexcept {
    return Elapsed() >= mTimeout;
  }

  /**
   * @brief Restarts the timer from the current clock reading.
   *
   * The timeout is unchanged; only the start point is reset.
   */
  void Reset() noexcept {
    mStart = mClock.Now();
  }

  /**
   * @brief Returns the configured timeout duration.
   */
  [[nodiscard]] Duration Timeout() const noexcept {
    return mTimeout;
  }

 private:
  const Clock& mClock;  ///< Injected clock (non-owning reference).
  TimePoint mStart;   ///< Time point of last construction or Reset().
  Duration mTimeout;  ///< Fixed duration until expiry.
};

}  // namespace Raptor

#endif  // RAPTOR_INTERFACES_TIMER_HPP

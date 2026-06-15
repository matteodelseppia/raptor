/**
 * @file fake_clock.hpp
 * @brief Deterministic test double for `Raptor::Clock`.
 *
 * `FakeClock` starts at an arbitrary fixed epoch and advances only
 * when `Advance()` is called.  This allows unit tests to control time
 * precisely without sleeping or relying on wall-clock behaviour.
 *
 * ## Usage
 *
 * @code
 * FakeClock clock;
 * Raptor::Timer t{clock, std::chrono::seconds{10}};
 *
 * EXPECT_FALSE(t.IsExpired());
 *
 * clock.Advance(std::chrono::seconds{9});
 * EXPECT_FALSE(t.IsExpired());
 *
 * clock.Advance(std::chrono::seconds{1});
 * EXPECT_TRUE(t.IsExpired());
 * @endcode
 */

#ifndef RAPTOR_TESTS_SUPPORT_FAKE_CLOCK_HPP
#define RAPTOR_TESTS_SUPPORT_FAKE_CLOCK_HPP

#include "raptor/interfaces/Clock.hpp"

namespace Raptor {

/**
 * @brief Advanceable test double for `Raptor::Clock`.
 *
 * The clock starts at `TimePoint{}` (the epoch of `steady_clock`)
 * and only moves forward via explicit `Advance()` calls.
 *
 * @note Not thread-safe — intended for single-threaded test fixtures
 * only.
 */
class FakeClock final : public Clock {
 public:
  FakeClock() = default;

  /**
   * @brief Returns the current fake time point.
   *
   * @return The fixed `TimePoint` set by the last `Advance()` call
   * (or `TimePoint{}` if `Advance()` has never been called).
   */
  [[nodiscard]] TimePoint Now() const override {
    return mNow;
  }

  /**
   * @brief Advances the fake clock by @p delta.
   *
   * @param delta  A positive duration to add to the current time.
   *               Passing a non-positive duration is legal but
   * unusual and will leave the clock unchanged or move it backward
   *               (the latter violates monotonicity, so avoid it in
   * tests that assert deadline behaviour).
   */
  void Advance(Duration delta) noexcept {
    mNow += delta;
  }

  /**
   * @brief Resets the fake clock back to the epoch (`TimePoint{}`).
   *
   * Useful for re-using a `FakeClock` across multiple independent
   * sub-tests within a single test case.
   */
  void Reset() noexcept {
    mNow = TimePoint{};
  }

 private:
  TimePoint
    mNow{};  ///< Current fake time; starts at the steady_clock epoch.
};

}  // namespace Raptor

#endif  // RAPTOR_TESTS_SUPPORT_FAKE_CLOCK_HPP

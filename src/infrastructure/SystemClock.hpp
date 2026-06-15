/**
 * @file system_clock.hpp
 * @brief Concrete `Clock` implementation backed by the OS real-time
 * clocks.
 *
 * `SystemClock` is the production implementation of `Raptor::Clock`.
 * It wraps `std::chrono::steady_clock` for monotonic `Now()` queries
 * and exposes an additional `WallNow()` helper (backed by
 * `std::chrono::system_clock`) for contexts that need a
 * human-readable wall-time timestamp, such as log formatting.
 *
 * ## Dependency note
 *
 * This header lives in `src/infrastructure/` and must never be
 * included from domain or interface headers.  Business logic depends
 * only on `raptor/interfaces/clock.hpp`.
 */

#ifndef RAPTOR_INFRASTRUCTURE_SYSTEM_CLOCK_HPP
#define RAPTOR_INFRASTRUCTURE_SYSTEM_CLOCK_HPP

#include "raptor/interfaces/Clock.hpp"

namespace Raptor {

/**
 * @brief Production clock backed by `std::chrono::steady_clock`.
 *
 * `Now()` always returns `std::chrono::steady_clock::now()`.  The
 * monotonicity guarantee is provided by the underlying platform
 * clock.
 *
 * `WallNow()` returns the current `system_clock::time_point` for use
 * only in logging or display contexts — it must not be used for
 * timeout or rate calculations because wall-time can jump.
 */
class SystemClock final : public Clock {
 public:
  SystemClock() = default;

  /**
   * @brief Returns the current monotonic time point.
   *
   * Delegates directly to `std::chrono::steady_clock::now()`.
   *
   * @return Current `TimePoint`.
   */
  [[nodiscard]] TimePoint Now() const override {
    return std::chrono::steady_clock::now();
  }

  /**
   * @brief Returns the current wall-clock time point.
   *
   * Intended for log timestamps and human-readable output only.
   * Do **not** use this for timeout calculations — use `Now()`.
   *
   * @return Current `std::chrono::system_clock::time_point`.
   */
  [[nodiscard]] static std::chrono::system_clock::time_point
  WallNow() {
    return std::chrono::system_clock::now();
  }
};

}  // namespace Raptor

#endif  // RAPTOR_INFRASTRUCTURE_SYSTEM_CLOCK_HPP

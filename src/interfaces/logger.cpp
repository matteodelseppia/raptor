/**
 * @file logger.cpp
 * @brief Global logger accessor and built-in no-op logger
 * implementation.
 *
 * The global logger pointer is stored as a std::atomic so that
 * SetLogger() and Log() are safe to call concurrently (typical during
 * startup / test fixture teardown while peer threads may still be
 * logging).
 *
 * The NullLogger is the default: it discards every message with zero
 * overhead so the program is safe to run before a real logger is
 * installed.
 */

#include "raptor/interfaces/logger.hpp"

#include <atomic>
#include <source_location>
#include <span>
#include <string_view>

namespace Raptor {

namespace {

// ---------------------------------------------------------------------------
// NullLogger — silent default; all methods are no-ops
// ---------------------------------------------------------------------------

/**
 * @brief Discards every log call silently.
 *
 * Active before SetLogger() installs a real adapter.  Also restored
 * when SetLogger(nullptr) is called (e.g. at the end of a test
 * fixture).
 */
class NullLogger final : public Logger {
 public:
  void Trace(std::string_view, std::source_location) override {
  }
  void Debug(std::string_view, std::source_location) override {
  }
  void Info(std::string_view, std::source_location) override {
  }
  void Warn(std::string_view, std::source_location) override {
  }
  void Error(std::string_view, std::source_location) override {
  }

  void Info(std::string_view, std::span<const Logger::Field>,
            std::source_location) override {
  }
  void Warn(std::string_view, std::span<const Logger::Field>,
            std::source_location) override {
  }
  void Error(std::string_view, std::span<const Logger::Field>,
             std::source_location) override {
  }
};

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------

/// @brief The singleton no-op logger returned when no adapter is
/// installed.
NullLogger
  gNullLogger{};  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

/**
 * @brief Atomic pointer to the active Logger.
 *
 * Initialised to &gNullLogger so Log() always returns a valid
 * reference without any startup ordering concerns.
 *
 * Relaxed loads are used in Log() (hot path) because the value only
 * changes at setup time and the caller is not synchronising anything
 * else on it. SetLogger() uses release to ensure the new logger's
 * construction is visible before any subsequent Log() calls.
 */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<Logger*> gActiveLogger{&gNullLogger};

}  // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

Logger& Log() noexcept {
  // Relaxed: we only need to read the pointer, not synchronise
  // any surrounding memory accesses.
  return *gActiveLogger.load(std::memory_order_relaxed);
}

void SetLogger(Logger* logger) noexcept {
  // Release: the new logger object must be fully visible to threads
  // that subsequently call Log().
  gActiveLogger.store(logger != nullptr ? logger : &gNullLogger,
                      std::memory_order_release);
}

}  // namespace Raptor

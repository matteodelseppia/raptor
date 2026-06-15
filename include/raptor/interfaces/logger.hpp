/**
 * @file Logger.hpp
 * @brief Abstract logging interface for Raptor.
 *
 * Business logic depends only on this header; no third-party logging
 * library is exposed here.  The concrete adapter (SpdlogLogger) lives
 * in infrastructure and is wired up at the composition root.
 *
 * ## Usage
 *
 * @code
 * // Plain message
 * Raptor::Log().Info("piece verified");
 *
 * // Structured fields
 * using F = Raptor::Logger::Field;
 * std::array fields{F{"peer", "192.168.1.1"}, F{"rate_kbps",
 * "1024"}}; Raptor::Log().Info("peer unchoked", fields);
 * @endcode
 *
 * ## Injection (tests)
 *
 * @code
 * MockLogger mock;
 * Raptor::SetLogger(&mock);
 * // ... exercise code under test ...
 * Raptor::SetLogger(nullptr); // restore noop logger
 * @endcode
 */

#ifndef RAPTOR_INTERFACES_LOGGER_HPP
#define RAPTOR_INTERFACES_LOGGER_HPP

#include <source_location>
#include <span>
#include <string>
#include <string_view>

namespace Raptor {

// ---------------------------------------------------------------------------
// Logger — abstract interface
// ---------------------------------------------------------------------------

/**
 * @brief Abstract logging interface.
 *
 * Provides five severity levels (Trace, Debug, Info, Warn, Error) in
 * both plain-message and structured-field forms.  The @p loc
 * parameter defaults to the call site so callers need not supply it
 * explicitly.
 *
 * Implementations must be thread-safe: the global logger may be
 * called concurrently from multiple peer connections.
 */
class Logger {
 public:
  // ---------------------------------------------------------------------------
  // Field — key/value pair for structured log entries
  // ---------------------------------------------------------------------------

  /**
   * @brief A key/value pair used to attach structured context to a
   * log entry.
   *
   * The key is a non-owning view; the value is an owned string so
   * callers can pass formatted numbers, addresses, or other dynamic
   * data.
   */
  struct Field {
    std::string_view
      mKey;  ///< Attribute name (e.g. "peer", "piece_idx").
    std::string
      mValue;  ///< Attribute value (e.g. "192.168.1.1", "42").
  };

  virtual ~Logger() = default;

  Logger() = default;
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

  // ---------------------------------------------------------------------------
  // Plain message overloads
  // ---------------------------------------------------------------------------

  /**
   * @brief Logs a trace-level message (very verbose, disabled in
   * release).
   * @param msg  Human-readable description.
   * @param loc  Call-site location (filled automatically by the
   * default).
   */
  virtual void Trace(
    std::string_view msg,
    std::source_location loc = std::source_location::current()) = 0;

  /**
   * @brief Logs a debug-level message.
   * @param msg  Human-readable description.
   * @param loc  Call-site location.
   */
  virtual void Debug(
    std::string_view msg,
    std::source_location loc = std::source_location::current()) = 0;

  /**
   * @brief Logs an info-level message.
   * @param msg  Human-readable description.
   * @param loc  Call-site location.
   */
  virtual void Info(
    std::string_view msg,
    std::source_location loc = std::source_location::current()) = 0;

  /**
   * @brief Logs a warning-level message.
   * @param msg  Human-readable description.
   * @param loc  Call-site location.
   */
  virtual void Warn(
    std::string_view msg,
    std::source_location loc = std::source_location::current()) = 0;

  /**
   * @brief Logs an error-level message.
   * @param msg  Human-readable description.
   * @param loc  Call-site location.
   */
  virtual void Error(
    std::string_view msg,
    std::source_location loc = std::source_location::current()) = 0;

  // ---------------------------------------------------------------------------
  // Structured-field overloads
  //
  // Prefer these when attaching measurable values (rates, counts,
  // peer IDs) so log aggregation tools can index them as discrete
  // fields rather than parsing free-form text.
  // ---------------------------------------------------------------------------

  /**
   * @brief Logs an info-level message with structured key/value
   * fields.
   * @param msg     Human-readable summary.
   * @param fields  Span of key/value pairs providing structured
   * context.
   * @param loc     Call-site location.
   */
  virtual void Info(
    std::string_view msg, std::span<const Field> fields,
    std::source_location loc = std::source_location::current()) = 0;

  /**
   * @brief Logs a warning-level message with structured key/value
   * fields.
   * @param msg     Human-readable summary.
   * @param fields  Span of key/value pairs providing structured
   * context.
   * @param loc     Call-site location.
   */
  virtual void Warn(
    std::string_view msg, std::span<const Field> fields,
    std::source_location loc = std::source_location::current()) = 0;

  /**
   * @brief Logs an error-level message with structured key/value
   * fields.
   * @param msg     Human-readable summary.
   * @param fields  Span of key/value pairs providing structured
   * context.
   * @param loc     Call-site location.
   */
  virtual void Error(
    std::string_view msg, std::span<const Field> fields,
    std::source_location loc = std::source_location::current()) = 0;
};

// ---------------------------------------------------------------------------
// Global accessor and injection point
// ---------------------------------------------------------------------------

/**
 * @brief Returns the process-wide active logger.
 *
 * Returns a no-op logger if no logger has been installed via
 * SetLogger(). The reference is stable for the lifetime of the
 * process; the pointed-to object is only replaced atomically by
 * SetLogger().
 *
 * @note Thread-safe: safe to call concurrently.
 */
[[nodiscard]] Logger& Log() noexcept;

/**
 * @brief Replaces the process-wide logger.
 *
 * @param logger  Pointer to the new logger.  Pass @c nullptr to
 * restore the built-in no-op logger.
 *
 * The caller must ensure @p logger outlives all subsequent calls to
 * Log(). Intended for use at the composition root (production) and in
 * test fixtures.
 */
void SetLogger(Logger* logger) noexcept;

}  // namespace Raptor

#endif  // RAPTOR_INTERFACES_LOGGER_HPP

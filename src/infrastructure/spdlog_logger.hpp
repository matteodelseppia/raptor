/**
 * @file spdlog_logger.hpp
 * @brief spdlog-backed Logger adapter declaration.
 *
 * Declared here with a pimpl so that spdlog headers remain confined
 * to the corresponding .cpp file.  Nothing outside of infrastructure/
 * need include this header.
 *
 * ### Default arguments on overrides
 * C++ default arguments are resolved at the call site using the
 * static type. To allow callers that hold a concrete `SpdlogLogger`
 * (e.g. tests) to omit `std::source_location`, the defaults are
 * repeated on every override here. Callers going through `Logger&` or
 * `Logger*` pick up the defaults from the base class as usual.
 */

#ifndef RAPTOR_INFRASTRUCTURE_SPDLOG_LOGGER_HPP
#define RAPTOR_INFRASTRUCTURE_SPDLOG_LOGGER_HPP

#include <memory>
#include <ostream>
#include <string_view>

#include "raptor/interfaces/logger.hpp"

namespace Raptor::Detail {

/**
 * @brief Logger adapter backed by spdlog.
 *
 * Routes Raptor log calls to spdlog sinks.  Two constructors are
 * provided:
 *
 * - **Default (console)**: writes coloured, human-readable output to
 * stderr via a spdlog stderr_color_sink.
 * - **Ostream**: writes to any `std::ostream` (used in tests to
 * capture output without touching the filesystem or console).
 *
 * Structured fields are appended after the message as `key=value`
 * pairs so the output remains human-readable and easily grep-able.
 *
 * ### Thread safety
 * Both constructors produce a multi-threaded spdlog logger (`_mt`
 * sinks), so concurrent calls from peer-connection threads are safe.
 *
 * ### spdlog confinement
 * spdlog headers are included only in `spdlog_logger.cpp`.  This
 * header exposes no spdlog types; the implementation is hidden behind
 * `Impl`.
 */
class SpdlogLogger final : public Logger {
 public:
  /**
   * @brief Constructs an spdlog logger writing to the console
   * (stderr).
   * @param name  Logger name shown in every log line (default:
   * "raptor").
   */
  explicit SpdlogLogger(std::string_view name = "raptor");

  /**
   * @brief Constructs an spdlog logger writing to the given ostream.
   *
   * Intended for unit tests: pass a `std::ostringstream` to capture
   * output without side effects.
   *
   * @param os    Output stream to write log lines to.
   * @param name  Logger name (default: "raptor").
   */
  explicit SpdlogLogger(std::ostream& outputStream,
                        std::string_view name = "raptor");

  ~SpdlogLogger() override;

  SpdlogLogger(const SpdlogLogger&) = delete;
  SpdlogLogger& operator=(const SpdlogLogger&) = delete;
  SpdlogLogger(SpdlogLogger&&) = delete;
  SpdlogLogger& operator=(SpdlogLogger&&) = delete;

  // Plain message overloads — default arg repeated so the concrete
  // type is callable without an explicit source_location argument.
  void Trace(std::string_view msg,
             std::source_location loc =
               std::source_location::current()) override;
  void Debug(std::string_view msg,
             std::source_location loc =
               std::source_location::current()) override;
  void Info(std::string_view msg,
            std::source_location loc =
              std::source_location::current()) override;
  void Warn(std::string_view msg,
            std::source_location loc =
              std::source_location::current()) override;
  void Error(std::string_view msg,
             std::source_location loc =
               std::source_location::current()) override;

  // Structured-field overloads.
  void Info(std::string_view msg,
            std::span<const Logger::Field> fields,
            std::source_location loc =
              std::source_location::current()) override;
  void Warn(std::string_view msg,
            std::span<const Logger::Field> fields,
            std::source_location loc =
              std::source_location::current()) override;
  void Error(std::string_view msg,
             std::span<const Logger::Field> fields,
             std::source_location loc =
               std::source_location::current()) override;

 private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

}  // namespace Raptor::Detail

#endif  // RAPTOR_INFRASTRUCTURE_SPDLOG_LOGGER_HPP

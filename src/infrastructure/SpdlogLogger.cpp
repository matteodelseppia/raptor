/**
 * @file spdlog_logger.cpp
 * @brief spdlog-backed Logger adapter implementation.
 *
 * spdlog headers are included ONLY here, keeping them out of every
 * other translation unit.  The rest of Raptor sees only
 * `Raptor::Logger`.
 *
 * ### Sink configuration
 * - Console constructor:  stderr colour sink (thread-safe, `_mt`).
 * - Ostream constructor:  ostream sink (thread-safe, `_mt`) — used in
 * tests.
 *
 * ### Structured fields
 * Fields are appended in `key=value` pairs separated by spaces, e.g.:
 *   "peer unchoked peer=192.168.1.1 rate_kbps=1024"
 * This keeps the output human-readable and grep-able without
 * requiring a JSON log aggregator.
 */

// ---- spdlog includes (confined to this file)
// --------------------------------
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
// -----------------------------------------------------------------------------

#include <cstddef>
#include <memory>
#include <ostream>
#include <source_location>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "raptor/interfaces/Logger.hpp"
#include "SpdlogLogger.hpp"

namespace Raptor::Detail {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

/**
 * @brief Appends structured fields to a message string.
 *
 * Produces `"<msg> key1=val1 key2=val2 ..."`.
 *
 * @param msg    Base message.
 * @param fields Key/value pairs to append.
 * @return Combined string ready to pass to spdlog.
 */
[[nodiscard]] std::string FormatWithFields(
  std::string_view msg, std::span<const Logger::Field> fields) {
  std::string out{msg};
  for (const auto& fld : fields) {
    out += ' ';
    out += fld.mKey;
    out += '=';
    out += fld.mValue;
  }
  return out;
}

}  // namespace

// ---------------------------------------------------------------------------
// Impl — hides spdlog types from the header
// ---------------------------------------------------------------------------

struct SpdlogLogger::Impl {
  std::shared_ptr<spdlog::logger> mLogger;
};

// ---------------------------------------------------------------------------
// Factory methods
// ---------------------------------------------------------------------------

std::unique_ptr<SpdlogLogger> SpdlogLogger::MakeConsole(
  std::string_view name) {
  return std::make_unique<SpdlogLogger>(name);
}

std::unique_ptr<SpdlogLogger> SpdlogLogger::MakeRotatingFile(
  std::string_view filePath, std::size_t maxFileSize,
  std::size_t maxFiles, std::string_view name) {
  // Use the private tag constructor so no temporary console sink is
  // created.
  return std::unique_ptr<SpdlogLogger>(new SpdlogLogger(
    RotatingFileTag{}, filePath, maxFileSize, maxFiles, name));
}

// ---------------------------------------------------------------------------
// Constructors / destructor
// ---------------------------------------------------------------------------

SpdlogLogger::SpdlogLogger(RotatingFileTag, std::string_view filePath,
                           std::size_t maxFileSize,
                           std::size_t maxFiles,
                           std::string_view name)
    : mImpl{std::make_unique<Impl>()} {
  auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
    std::string{filePath}, maxFileSize, maxFiles);
  mImpl->mLogger = std::make_shared<spdlog::logger>(std::string{name},
                                                    std::move(sink));
  mImpl->mLogger->set_level(spdlog::level::trace);
  mImpl->mLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
}

SpdlogLogger::SpdlogLogger(std::string_view name)
    : mImpl{std::make_unique<Impl>()} {
  auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
  mImpl->mLogger = std::make_shared<spdlog::logger>(std::string{name},
                                                    std::move(sink));
  mImpl->mLogger->set_level(spdlog::level::trace);
  // Pattern: [timestamp] [logger] [level] message
  mImpl->mLogger->set_pattern(
    "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
}

SpdlogLogger::SpdlogLogger(std::ostream& outputStream,
                           std::string_view name)
    : mImpl{std::make_unique<Impl>()} {
  auto sink =
    std::make_shared<spdlog::sinks::ostream_sink_mt>(outputStream);
  mImpl->mLogger = std::make_shared<spdlog::logger>(std::string{name},
                                                    std::move(sink));
  mImpl->mLogger->set_level(spdlog::level::trace);
  // Simpler pattern for tests (no ANSI colour codes)
  mImpl->mLogger->set_pattern("[%l] %v");
}

// Must be defined here so unique_ptr<Impl> destructor can see Impl's
// definition.
SpdlogLogger::~SpdlogLogger() = default;

// ---------------------------------------------------------------------------
// Plain-message overloads
// ---------------------------------------------------------------------------

void SpdlogLogger::Trace(std::string_view msg,
                         std::source_location /*loc*/) {
  mImpl->mLogger->trace(msg);
}

void SpdlogLogger::Debug(std::string_view msg,
                         std::source_location /*loc*/) {
  mImpl->mLogger->debug(msg);
}

void SpdlogLogger::Info(std::string_view msg,
                        std::source_location /*loc*/) {
  mImpl->mLogger->info(msg);
}

void SpdlogLogger::Warn(std::string_view msg,
                        std::source_location /*loc*/) {
  mImpl->mLogger->warn(msg);
}

void SpdlogLogger::Error(std::string_view msg,
                         std::source_location /*loc*/) {
  mImpl->mLogger->error(msg);
}

// ---------------------------------------------------------------------------
// Structured-field overloads
// ---------------------------------------------------------------------------

void SpdlogLogger::Info(std::string_view msg,
                        std::span<const Logger::Field> fields,
                        std::source_location /*loc*/) {
  mImpl->mLogger->info(FormatWithFields(msg, fields));
}

void SpdlogLogger::Warn(std::string_view msg,
                        std::span<const Logger::Field> fields,
                        std::source_location /*loc*/) {
  mImpl->mLogger->warn(FormatWithFields(msg, fields));
}

void SpdlogLogger::Error(std::string_view msg,
                         std::span<const Logger::Field> fields,
                         std::source_location /*loc*/) {
  mImpl->mLogger->error(FormatWithFields(msg, fields));
}

}  // namespace Raptor::Detail

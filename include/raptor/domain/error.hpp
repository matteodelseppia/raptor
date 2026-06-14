/**
 * @file error.hpp
 * @brief Error handling conventions and Result type for Raptor.
 *
 * ## Policy
 *
 * - **Expected / recoverable failures** (bad input, network errors,
 * I/O errors, protocol violations) are communicated via `Result<T>`
 * or `Status`. Callers must inspect the value; nothing is silently
 * swallowed.
 * - **Truly exceptional / programmer errors** (violated
 * preconditions, logic bugs) may be expressed with exceptions or
 * assertions.  These must **not** cross interface boundaries as
 * `std::exception`; callers of interfaces receive a `Result`.
 *
 * ## Usage
 *
 * @code
 * Result<int> Parse(std::string_view input) {
 *     if (input.empty()) {
 *         return std::unexpected(Fail(Errc::InvalidBencode, "empty
 * input"));
 *     }
 *     return 42;
 * }
 *
 * auto result = Parse(data);
 * if (!result) {
 *     log(result.error().Message());
 * }
 * @endcode
 */

#ifndef RAPTOR_DOMAIN_ERROR_HPP
#define RAPTOR_DOMAIN_ERROR_HPP

#include <expected>
#include <string>
#include <string_view>
#include <system_error>

namespace Raptor {

// ---------------------------------------------------------------------------
// Errc — domain error categories
// ---------------------------------------------------------------------------

/**
 * @brief Domain-level error categories for Raptor.
 *
 * Each enumerator represents a distinct class of failure.
 * Fine-grained context is carried by `Error::Message()`.
 */
enum class Errc {
  InvalidBencode,   ///< Bencoded data is malformed or truncated.
  InvalidMetainfo,  ///< Torrent metainfo violates the spec.
  Io,               ///< File-system or disk I/O error.
  Network,          ///< Transport-level network error.
  Protocol,         ///< BitTorrent protocol violation.
  HashMismatch,     ///< Piece SHA-1 verification failed.
  Cancelled,        ///< Operation was explicitly cancelled.
};

// ---------------------------------------------------------------------------
// Error — value type carrying an Errc and optional context string
// ---------------------------------------------------------------------------

/**
 * @brief Lightweight error value returned by Raptor operations.
 *
 * Carries an `Errc` category and an optional human-readable context
 * string. Cheap to copy; equality is defined so tests can use
 * EXPECT_EQ.
 */
class Error {
 public:
  /**
   * @brief Constructs an Error with the given category and optional
   * message.
   * @param code     The error category.
   * @param message  Optional free-form context string.
   */
  explicit Error(Errc code, std::string message = {}) noexcept
      : mCode{code}, mMessage{std::move(message)} {
  }

  /// @brief Returns the error category.
  [[nodiscard]] Errc Code() const noexcept {
    return mCode;
  }

  /// @brief Returns the optional context message (may be empty).
  [[nodiscard]] const std::string& Message() const noexcept {
    return mMessage;
  }

  /**
   * @brief Converts to a `std::error_code` for interop with system
   * APIs.
   *
   * Uses the Raptor error category returned by `RaptorCategory()`.
   */
  [[nodiscard]] std::error_code ToErrorCode() const noexcept;

  /// @brief Value equality — both category and message must match.
  [[nodiscard]] bool operator==(const Error&) const noexcept =
    default;

 private:
  Errc mCode;
  std::string mMessage;
};

// ---------------------------------------------------------------------------
// Result<T> and Status aliases
// ---------------------------------------------------------------------------

/**
 * @brief A value-or-error type used as the primary return type for
 * fallible Raptor operations.
 *
 * On success, holds a `T`.  On failure, holds an `Error`.  Supports
 * monadic chaining via `and_then`, `transform`, `or_else` (C++23
 * `std::expected`).
 *
 * @tparam T The success value type.
 */
template <class T>
using Result = std::expected<T, Error>;

/**
 * @brief Convenience alias for operations that succeed with no value.
 *
 * Equivalent to `Result<void>`.
 */
using Status = Result<void>;

// ---------------------------------------------------------------------------
// Helper makers
// ---------------------------------------------------------------------------

/**
 * @brief Constructs an `Error` and wraps it in `std::unexpected` for
 * direct use in `return` statements.
 *
 * @param code     The error category.
 * @param message  Optional context string.
 * @return `std::unexpected<Error>` ready to initialise any
 * `Result<T>`.
 *
 * @code
 * return Fail(Errc::Io, "cannot open file");
 * @endcode
 */
[[nodiscard]] inline std::unexpected<Error> Fail(
  Errc code, std::string message = {}) {
  return std::unexpected<Error>{Error{code, std::move(message)}};
}

// ---------------------------------------------------------------------------
// std::error_category / std::error_code interop
// ---------------------------------------------------------------------------

/**
 * @brief Returns the singleton `std::error_category` for Raptor
 * errors.
 *
 * Maps each `Errc` enumerator to a short English description via
 * `error_category::message()`.
 */
[[nodiscard]] const std::error_category& RaptorCategory() noexcept;

/**
 * @brief Creates a `std::error_code` from an `Errc` using
 * `RaptorCategory()`.
 */
[[nodiscard]] inline std::error_code MakeErrorCode(
  Errc errc) noexcept {
  return {static_cast<int>(errc), RaptorCategory()};
}

}  // namespace Raptor

// ---------------------------------------------------------------------------
// ADL hook so std::error_code{some_errc} works directly
// ---------------------------------------------------------------------------

template <>
struct std::is_error_code_enum<Raptor::Errc> : std::true_type {};

namespace std {
[[nodiscard]] inline error_code make_error_code(
  Raptor::Errc errc) noexcept {
  return Raptor::MakeErrorCode(errc);
}
}  // namespace std

#endif  // RAPTOR_DOMAIN_ERROR_HPP

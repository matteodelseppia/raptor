/**
 * @file error.cpp
 * @brief Implementation of the Raptor error category and Error
 * interop helpers.
 */

#include "raptor/domain/error.hpp"

#include <string>

namespace Raptor {

namespace {

/**
 * @brief Concrete `std::error_category` implementation for Raptor
 * errors.
 *
 * Provides human-readable descriptions for each `Errc` enumerator.
 * The singleton is returned by `RaptorCategory()`.
 */
class RaptorErrorCategory final : public std::error_category {
 public:
  /// @brief Returns the category name used in diagnostic output.
  [[nodiscard]] const char* name() const noexcept override {
    return "raptor";
  }

  /**
   * @brief Maps an `Errc` integer value to a descriptive string.
   *
   * Returns a non-empty string for every defined enumerator; falls
   * back to "unknown raptor error" for any value outside the
   * enumeration so the function is safe to call with arbitrary
   * integers from legacy interop.
   *
   * @param condition Integer cast of an `Errc` enumerator.
   * @return Human-readable error description.
   */
  [[nodiscard]] std::string message(int condition) const override {
    switch (static_cast<Errc>(condition)) {
      case Errc::InvalidBencode:
        return "invalid or malformed bencoded data";
      case Errc::InvalidMetainfo:
        return "invalid torrent metainfo";
      case Errc::Io:
        return "I/O error";
      case Errc::Network:
        return "network error";
      case Errc::Protocol:
        return "BitTorrent protocol violation";
      case Errc::HashMismatch:
        return "piece hash mismatch";
      case Errc::Cancelled:
        return "operation cancelled";
    }
    return "unknown raptor error";
  }
};

}  // namespace

const std::error_category& RaptorCategory() noexcept {
  static const RaptorErrorCategory instance{};
  return instance;
}

std::error_code Error::ToErrorCode() const noexcept {
  return MakeErrorCode(mCode);
}

}  // namespace Raptor

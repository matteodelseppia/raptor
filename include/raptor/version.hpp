#pragma once

#include <string_view>

namespace Raptor {

/**
 * @brief Returns the current Raptor version string.
 *
 * Provides a non-empty, human-readable version identifier for smoke testing
 * and diagnostic output. The format is MAJOR.MINOR.PATCH.
 *
 * @return A non-empty string_view identifying the build version.
 */
[[nodiscard]] std::string_view Version() noexcept;

} // namespace Raptor

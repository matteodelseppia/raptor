#include <gtest/gtest.h>

#include "raptor/Version.hpp"

namespace {

/**
 * @brief Smoke test — verifies that raptor_core links correctly and
 * that Raptor::Version() returns a non-empty string.
 *
 * This is the canary test: if it runs, the GTest/CTest pipeline, the
 * raptor_core library, and the public include path are all wired up
 * correctly.
 */
TEST(Smoke, BuildLinks) {
  const std::string_view v = Raptor::Version();
  EXPECT_FALSE(v.empty())
    << "Raptor::Version() must return a non-empty string";
}

}  // namespace

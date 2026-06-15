/**
 * @file error_test.cpp
 * @brief Unit tests for Raptor::Error, Raptor::Result, and
 * Raptor::Status.
 *
 * Tests cover:
 * - Constructing success and failure Result<T> values.
 * - Error equality.
 * - Error code and message propagation.
 * - Monadic chaining (and_then, transform, or_else).
 * - RaptorCategory message strings.
 * - Fail() helper.
 * - std::error_code interop via MakeErrorCode and the ADL hook.
 */

#include <gtest/gtest.h>

#include <string>

#include "raptor/domain/Error.hpp"

using Raptor::Errc;
using Raptor::Error;
using Raptor::Fail;
using Raptor::MakeErrorCode;
using Raptor::RaptorCategory;
using Raptor::Result;
using Raptor::Status;

// ---------------------------------------------------------------------------
// Error construction and accessors
// ---------------------------------------------------------------------------

TEST(ErrorTest, CodeAccessor) {
  const Error err{Errc::Io, "disk full"};
  EXPECT_EQ(err.Code(), Errc::Io);
}

TEST(ErrorTest, MessageAccessor) {
  const Error err{Errc::Network, "connection refused"};
  EXPECT_EQ(err.Message(), "connection refused");
}

TEST(ErrorTest, DefaultMessageIsEmpty) {
  const Error err{Errc::Cancelled};
  EXPECT_TRUE(err.Message().empty());
}

// ---------------------------------------------------------------------------
// Equality
// ---------------------------------------------------------------------------

TEST(ErrorTest, EqualityMatchesCodeAndMessage) {
  const Error lhs{Errc::HashMismatch, "piece 3"};
  const Error rhs{Errc::HashMismatch, "piece 3"};
  EXPECT_EQ(lhs, rhs);
}

TEST(ErrorTest, InequalityOnDifferentCode) {
  const Error lhs{Errc::Io};
  const Error rhs{Errc::Network};
  EXPECT_NE(lhs, rhs);
}

TEST(ErrorTest, InequalityOnDifferentMessage) {
  const Error lhs{Errc::Io, "msg-a"};
  const Error rhs{Errc::Io, "msg-b"};
  EXPECT_NE(lhs, rhs);
}

// ---------------------------------------------------------------------------
// Result<T> — success path
// ---------------------------------------------------------------------------

TEST(ResultTest, SuccessHasValue) {
  const Result<int> result{42};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 42);
}

TEST(ResultTest, SuccessStringValue) {
  const Result<std::string> result{"hello"};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, "hello");
}

// ---------------------------------------------------------------------------
// Result<T> — failure path
// ---------------------------------------------------------------------------

TEST(ResultTest, FailureHasNoValue) {
  Result<int> result = Fail(Errc::InvalidBencode, "bad byte");
  EXPECT_FALSE(result.has_value());
}

TEST(ResultTest, FailurePropagatesCode) {
  Result<int> result = Fail(Errc::Protocol, "unexpected message");
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error().Code(), Errc::Protocol);
}

TEST(ResultTest, FailurePropagatesMessage) {
  Result<int> result = Fail(Errc::Io, "read failed");
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error().Message(), "read failed");
}

// ---------------------------------------------------------------------------
// Status (Result<void>)
// ---------------------------------------------------------------------------

TEST(StatusTest, SuccessStatus) {
  const Status status{};
  EXPECT_TRUE(status.has_value());
}

TEST(StatusTest, FailureStatus) {
  Status status = Fail(Errc::Cancelled);
  EXPECT_FALSE(status.has_value());
  EXPECT_EQ(status.error().Code(), Errc::Cancelled);
}

// ---------------------------------------------------------------------------
// Monadic chaining
// ---------------------------------------------------------------------------

TEST(ResultTest, AndThenOnSuccess) {
  const Result<int> start{10};
  auto chained = start.and_then([](int val) -> Result<int> {
    return val * 2;
  });
  ASSERT_TRUE(chained.has_value());
  EXPECT_EQ(*chained, 20);
}

TEST(ResultTest, AndThenShortCircuitsOnFailure) {
  Result<int> start = Fail(Errc::Io, "error");
  bool called = false;
  auto chained =
    start.and_then([&called](int /*val*/) -> Result<int> {
      called = true;
      return 99;
    });
  EXPECT_FALSE(chained.has_value());
  EXPECT_FALSE(called);
  EXPECT_EQ(chained.error().Code(), Errc::Io);
}

TEST(ResultTest, TransformOnSuccess) {
  const Result<int> start{5};
  auto mapped = start.transform([](int val) {
    return val + 1;
  });
  ASSERT_TRUE(mapped.has_value());
  EXPECT_EQ(*mapped, 6);
}

TEST(ResultTest, TransformShortCircuitsOnFailure) {
  Result<int> start = Fail(Errc::Network);
  auto mapped = start.transform([](int val) {
    return val + 1;
  });
  EXPECT_FALSE(mapped.has_value());
  EXPECT_EQ(mapped.error().Code(), Errc::Network);
}

TEST(ResultTest, OrElseOnFailureRecovers) {
  Result<int> start = Fail(Errc::Cancelled);
  auto recovered =
    start.or_else([](const Error& /*err*/) -> Result<int> {
      return -1;
    });
  ASSERT_TRUE(recovered.has_value());
  EXPECT_EQ(*recovered, -1);
}

TEST(ResultTest, OrElseNotCalledOnSuccess) {
  const Result<int> start{7};
  bool called = false;
  auto result =
    start.or_else([&called](const Error& /*err*/) -> Result<int> {
      called = true;
      return -1;
    });
  EXPECT_TRUE(result.has_value());
  EXPECT_FALSE(called);
}

// ---------------------------------------------------------------------------
// RaptorCategory — human-readable messages
// ---------------------------------------------------------------------------

TEST(RaptorCategoryTest, Name) {
  EXPECT_STREQ(RaptorCategory().name(), "raptor");
}

TEST(RaptorCategoryTest, InvalidBencodeMessage) {
  const auto ec = MakeErrorCode(Errc::InvalidBencode);
  EXPECT_FALSE(ec.message().empty());
  EXPECT_EQ(ec.message(), "invalid or malformed bencoded data");
}

TEST(RaptorCategoryTest, InvalidMetainfoMessage) {
  EXPECT_FALSE(
    MakeErrorCode(Errc::InvalidMetainfo).message().empty());
}

TEST(RaptorCategoryTest, IoMessage) {
  EXPECT_EQ(MakeErrorCode(Errc::Io).message(), "I/O error");
}

TEST(RaptorCategoryTest, NetworkMessage) {
  EXPECT_FALSE(MakeErrorCode(Errc::Network).message().empty());
}

TEST(RaptorCategoryTest, ProtocolMessage) {
  EXPECT_FALSE(MakeErrorCode(Errc::Protocol).message().empty());
}

TEST(RaptorCategoryTest, HashMismatchMessage) {
  EXPECT_FALSE(MakeErrorCode(Errc::HashMismatch).message().empty());
}

TEST(RaptorCategoryTest, CancelledMessage) {
  EXPECT_EQ(MakeErrorCode(Errc::Cancelled).message(),
            "operation cancelled");
}

// ---------------------------------------------------------------------------
// std::error_code interop
// ---------------------------------------------------------------------------

TEST(ErrorCodeInteropTest, ToErrorCodeCategory) {
  const Error err{Errc::Protocol, "bad handshake"};
  const auto ec = err.ToErrorCode();
  EXPECT_EQ(&ec.category(), &RaptorCategory());
}

TEST(ErrorCodeInteropTest, ToErrorCodeValue) {
  const Error err{Errc::HashMismatch};
  const auto ec = err.ToErrorCode();
  EXPECT_EQ(ec.value(), static_cast<int>(Errc::HashMismatch));
}

TEST(ErrorCodeInteropTest, AdlMakeErrorCode) {
  // std::error_code constructed via ADL hook (is_error_code_enum
  // specialisation)
  const std::error_code ec = Errc::InvalidMetainfo;
  EXPECT_EQ(&ec.category(), &RaptorCategory());
  EXPECT_EQ(ec.value(), static_cast<int>(Errc::InvalidMetainfo));
}

TEST(ErrorCodeInteropTest, MakeErrorCodeMatchesDirectCast) {
  for (const auto errc : {Errc::InvalidBencode, Errc::InvalidMetainfo,
                          Errc::Io, Errc::Network, Errc::Protocol,
                          Errc::HashMismatch, Errc::Cancelled}) {
    const auto ec = MakeErrorCode(errc);
    EXPECT_EQ(ec.value(), static_cast<int>(errc));
    EXPECT_EQ(&ec.category(), &RaptorCategory());
  }
}

// ---------------------------------------------------------------------------
// Fail() helper
// ---------------------------------------------------------------------------

TEST(FailTest, ReturnsUnexpectedWithCorrectCode) {
  Result<double> res = Fail(Errc::Io, "write failed");
  ASSERT_FALSE(res.has_value());
  EXPECT_EQ(res.error().Code(), Errc::Io);
  EXPECT_EQ(res.error().Message(), "write failed");
}

TEST(FailTest, WorksForStatusType) {
  Status status = Fail(Errc::Network, "timeout");
  ASSERT_FALSE(status.has_value());
  EXPECT_EQ(status.error().Code(), Errc::Network);
}

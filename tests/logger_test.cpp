/**
 * @file logger_test.cpp
 * @brief Unit tests for the Raptor logging subsystem.
 *
 * Tests cover:
 * - MockLogger via GMock: verify call routing and level correctness.
 * - SetLogger / Log() global injection and restoration.
 * - SpdlogLogger ostream-sink output: verify messages and structured
 * fields reach the stream at the correct level prefix.
 * - NullLogger (default): Log() is callable before any adapter is
 * installed.
 */

#include "raptor/interfaces/logger.hpp"

// Include the infrastructure adapter directly; tests are the one
// place allowed to know about concrete implementations.
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <sstream>
#include <string>

#include "../src/infrastructure/spdlog_logger.hpp"

using ::testing::HasSubstr;
using ::testing::InSequence;

namespace Raptor {

// ---------------------------------------------------------------------------
// MockLogger — GMock-based test double
// ---------------------------------------------------------------------------

/**
 * @brief GMock double for Raptor::Logger.
 *
 * Use EXPECT_CALL to assert that production code emits the right log
 * events at the right levels.
 */
class MockLogger : public Logger {
 public:
  MOCK_METHOD(void, Trace,
              (std::string_view msg, std::source_location loc),
              (override));
  MOCK_METHOD(void, Debug,
              (std::string_view msg, std::source_location loc),
              (override));
  MOCK_METHOD(void, Info,
              (std::string_view msg, std::source_location loc),
              (override));
  MOCK_METHOD(void, Warn,
              (std::string_view msg, std::source_location loc),
              (override));
  MOCK_METHOD(void, Error,
              (std::string_view msg, std::source_location loc),
              (override));

  MOCK_METHOD(void, Info,
              (std::string_view msg,
               std::span<const Logger::Field> fields,
               std::source_location loc),
              (override));
  MOCK_METHOD(void, Warn,
              (std::string_view msg,
               std::span<const Logger::Field> fields,
               std::source_location loc),
              (override));
  MOCK_METHOD(void, Error,
              (std::string_view msg,
               std::span<const Logger::Field> fields,
               std::source_location loc),
              (override));
};

// ---------------------------------------------------------------------------
// Fixture — installs a mock logger, restores the null logger on
// teardown
// ---------------------------------------------------------------------------

class LoggerInjectionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    SetLogger(&mMock);
  }

  void TearDown() override {
    SetLogger(nullptr);  // restore null logger
  }

  MockLogger mMock;
};

// ---------------------------------------------------------------------------
// Global accessor tests
// ---------------------------------------------------------------------------

TEST(LoggerGlobalTest, LogReturnsSomethingBeforeSetLogger) {
  // Should not crash; the null logger is the default.
  EXPECT_NO_THROW(Log().Info("startup"));
}

TEST(LoggerGlobalTest, SetLoggerNullptrRestoresNullLogger) {
  MockLogger mock;
  SetLogger(&mock);
  SetLogger(nullptr);
  // After restoring, no call should reach the mock.
  EXPECT_CALL(mock, Info(testing::_, testing::_)).Times(0);
  Log().Info("after restore");
}

// ---------------------------------------------------------------------------
// MockLogger injection tests — verify level routing
// ---------------------------------------------------------------------------

TEST_F(LoggerInjectionTest, TraceRouted) {
  EXPECT_CALL(mMock, Trace(std::string_view{"trace msg"}, testing::_))
    .Times(1);
  Log().Trace("trace msg");
}

TEST_F(LoggerInjectionTest, DebugRouted) {
  EXPECT_CALL(mMock, Debug(std::string_view{"debug msg"}, testing::_))
    .Times(1);
  Log().Debug("debug msg");
}

TEST_F(LoggerInjectionTest, InfoRouted) {
  EXPECT_CALL(mMock, Info(std::string_view{"info msg"}, testing::_))
    .Times(1);
  Log().Info("info msg");
}

TEST_F(LoggerInjectionTest, WarnRouted) {
  EXPECT_CALL(mMock, Warn(std::string_view{"warn msg"}, testing::_))
    .Times(1);
  Log().Warn("warn msg");
}

TEST_F(LoggerInjectionTest, ErrorRouted) {
  EXPECT_CALL(mMock, Error(std::string_view{"error msg"}, testing::_))
    .Times(1);
  Log().Error("error msg");
}

TEST_F(LoggerInjectionTest, StructuredInfoRouted) {
  EXPECT_CALL(mMock, Info(std::string_view{"peer unchoked"},
                          testing::_, testing::_))
    .Times(1);

  std::array<Logger::Field, 1> fields{
    Logger::Field{"peer", "10.0.0.1"}};
  Log().Info("peer unchoked", fields);
}

TEST_F(LoggerInjectionTest, StructuredWarnRouted) {
  EXPECT_CALL(mMock, Warn(std::string_view{"slow peer"}, testing::_,
                          testing::_))
    .Times(1);

  std::array<Logger::Field, 1> fields{
    Logger::Field{"rate_kbps", "4"}};
  Log().Warn("slow peer", fields);
}

TEST_F(LoggerInjectionTest, StructuredErrorRouted) {
  EXPECT_CALL(mMock, Error(std::string_view{"hash mismatch"},
                           testing::_, testing::_))
    .Times(1);

  std::array<Logger::Field, 2> fields{
    Logger::Field{"piece", "7"},
    Logger::Field{"peer", "10.0.0.2"},
  };
  Log().Error("hash mismatch", fields);
}

TEST_F(LoggerInjectionTest, CallsArrivedInOrder) {
  InSequence seq;
  EXPECT_CALL(mMock, Info(std::string_view{"first"}, testing::_));
  EXPECT_CALL(mMock, Warn(std::string_view{"second"}, testing::_));
  EXPECT_CALL(mMock, Error(std::string_view{"third"}, testing::_));

  Log().Info("first");
  Log().Warn("second");
  Log().Error("third");
}

// ---------------------------------------------------------------------------
// SpdlogLogger — ostream sink tests
// ---------------------------------------------------------------------------

class SpdlogLoggerTest : public ::testing::Test {
 protected:
  std::ostringstream mStream;
  Raptor::Detail::SpdlogLogger mLogger{mStream, "test"};
};

TEST_F(SpdlogLoggerTest, InfoAppearsInStream) {
  mLogger.Info("download started");
  EXPECT_THAT(mStream.str(), HasSubstr("download started"));
}

TEST_F(SpdlogLoggerTest, InfoLevelPrefixPresent) {
  mLogger.Info("hello");
  // The test pattern is "[level] message"
  EXPECT_THAT(mStream.str(), HasSubstr("[info]"));
}

TEST_F(SpdlogLoggerTest, WarnLevelPrefixPresent) {
  mLogger.Warn("low bandwidth");
  EXPECT_THAT(mStream.str(), HasSubstr("[warning]"));
}

TEST_F(SpdlogLoggerTest, ErrorLevelPrefixPresent) {
  mLogger.Error("piece hash mismatch");
  EXPECT_THAT(mStream.str(), HasSubstr("[error]"));
}

TEST_F(SpdlogLoggerTest, TraceLevelPrefixPresent) {
  mLogger.Trace("verbose detail");
  EXPECT_THAT(mStream.str(), HasSubstr("[trace]"));
}

TEST_F(SpdlogLoggerTest, DebugLevelPrefixPresent) {
  mLogger.Debug("scheduler state");
  EXPECT_THAT(mStream.str(), HasSubstr("[debug]"));
}

TEST_F(SpdlogLoggerTest, StructuredInfoAppendsFields) {
  std::array<Logger::Field, 2> fields{
    Logger::Field{"peer", "192.168.1.1"},
    Logger::Field{"rate_kbps", "1024"},
  };
  mLogger.Info("peer unchoked", fields);

  const auto out = mStream.str();
  EXPECT_THAT(out, HasSubstr("peer unchoked"));
  EXPECT_THAT(out, HasSubstr("peer=192.168.1.1"));
  EXPECT_THAT(out, HasSubstr("rate_kbps=1024"));
}

TEST_F(SpdlogLoggerTest, StructuredWarnAppendsFields) {
  std::array<Logger::Field, 1> fields{Logger::Field{"piece", "42"}};
  mLogger.Warn("piece timeout", fields);

  EXPECT_THAT(mStream.str(), HasSubstr("piece=42"));
}

TEST_F(SpdlogLoggerTest, StructuredErrorAppendsFields) {
  std::array<Logger::Field, 1> fields{
    Logger::Field{"expected", "abc123"}};
  mLogger.Error("hash mismatch", fields);

  EXPECT_THAT(mStream.str(), HasSubstr("expected=abc123"));
}

TEST_F(SpdlogLoggerTest, MultipleMessagesAllAppear) {
  mLogger.Info("msg one");
  mLogger.Warn("msg two");
  mLogger.Error("msg three");

  const auto out = mStream.str();
  EXPECT_THAT(out, HasSubstr("msg one"));
  EXPECT_THAT(out, HasSubstr("msg two"));
  EXPECT_THAT(out, HasSubstr("msg three"));
}

}  // namespace Raptor

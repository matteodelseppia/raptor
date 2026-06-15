/**
 * @file clock_test.cpp
 * @brief Unit tests for Clock, FakeClock, SystemClock, and Timer.
 */

#include "raptor/interfaces/Clock.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "infrastructure/SystemClock.hpp"
#include "raptor/interfaces/Timer.hpp"
#include "support/FakeClock.hpp"

using namespace std::chrono_literals;
using Raptor::Duration;
using Raptor::FakeClock;
using Raptor::SystemClock;
using Raptor::TimePoint;
using Raptor::Timer;

// ---------------------------------------------------------------------------
// FakeClock tests
// ---------------------------------------------------------------------------

TEST(FakeClockTest, StartsAtEpoch) {
  FakeClock clock;
  EXPECT_EQ(clock.Now(), TimePoint{});
}

TEST(FakeClockTest, AdvanceMovesClock) {
  FakeClock clock;
  clock.Advance(10s);
  EXPECT_EQ(clock.Now(), TimePoint{} + 10s);
}

TEST(FakeClockTest, AdvanceIsAdditive) {
  FakeClock clock;
  clock.Advance(3s);
  clock.Advance(7s);
  EXPECT_EQ(clock.Now(), TimePoint{} + 10s);
}

TEST(FakeClockTest, ResetReturnsToEpoch) {
  FakeClock clock;
  clock.Advance(100s);
  clock.Reset();
  EXPECT_EQ(clock.Now(), TimePoint{});
}

TEST(FakeClockTest, MultipleNowCallsReturnSameValue) {
  FakeClock clock;
  clock.Advance(5s);
  EXPECT_EQ(clock.Now(), clock.Now());
}

// ---------------------------------------------------------------------------
// SystemClock tests
// ---------------------------------------------------------------------------

TEST(SystemClockTest, NowReturnsNonZeroTimePoint) {
  SystemClock clock;
  // steady_clock has been running since boot; its value is large.
  EXPECT_GT(clock.Now().time_since_epoch().count(), 0);
}

TEST(SystemClockTest, SuccessiveCallsAreNonDecreasing) {
  SystemClock clock;
  const auto t1 = clock.Now();
  const auto t2 = clock.Now();
  EXPECT_GE(t2, t1);
}

TEST(SystemClockTest, WallNowReturnsNonZeroTimePoint) {
  EXPECT_GT(SystemClock::WallNow().time_since_epoch().count(), 0);
}

// ---------------------------------------------------------------------------
// Timer tests
// ---------------------------------------------------------------------------

TEST(TimerTest, NotExpiredBeforeTimeout) {
  FakeClock clock;
  Timer t{clock, 10s};
  EXPECT_FALSE(t.IsExpired());
}

TEST(TimerTest, ExpiredAfterTimeout) {
  FakeClock clock;
  Timer t{clock, 10s};
  clock.Advance(10s);
  EXPECT_TRUE(t.IsExpired());
}

TEST(TimerTest, NotExpiredJustBeforeTimeout) {
  FakeClock clock;
  Timer t{clock, 10s};
  clock.Advance(9s + 999ms);
  EXPECT_FALSE(t.IsExpired());
}

TEST(TimerTest, ExpiredWhenExceedingTimeout) {
  FakeClock clock;
  Timer t{clock, 10s};
  clock.Advance(100s);
  EXPECT_TRUE(t.IsExpired());
}

TEST(TimerTest, ElapsedMatchesAdvance) {
  FakeClock clock;
  Timer t{clock, 30s};
  clock.Advance(7s);
  EXPECT_EQ(t.Elapsed(), 7s);
}

TEST(TimerTest, RemainingDecreasesAsTimeAdvances) {
  FakeClock clock;
  Timer t{clock, 30s};

  clock.Advance(10s);
  EXPECT_EQ(t.Remaining(), 20s);

  clock.Advance(10s);
  EXPECT_EQ(t.Remaining(), 10s);
}

TEST(TimerTest, RemainingIsZeroOnceExpired) {
  FakeClock clock;
  Timer t{clock, 10s};
  clock.Advance(50s);
  EXPECT_EQ(t.Remaining(), Duration::zero());
}

TEST(TimerTest, ResetRestartsClock) {
  FakeClock clock;
  Timer t{clock, 10s};
  clock.Advance(10s);
  ASSERT_TRUE(t.IsExpired());

  t.Reset();
  EXPECT_FALSE(t.IsExpired());
  EXPECT_EQ(t.Elapsed(), Duration::zero());
  EXPECT_EQ(t.Remaining(), 10s);
}

TEST(TimerTest, TimeoutAccessor) {
  FakeClock clock;
  Timer t{clock, 42s};
  EXPECT_EQ(t.Timeout(), 42s);
}

TEST(TimerTest, ElapsedAtZeroAtStart) {
  FakeClock clock;
  Timer t{clock, 10s};
  EXPECT_EQ(t.Elapsed(), Duration::zero());
}

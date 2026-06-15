/**
 * @file file_system_test.cpp
 * @brief Behavioural test suite for `FileSystem` implementations.
 *
 * The same parametrized suite is run against both
 * `InMemoryFileSystem` and `StdFileSystem`, verifying that the two
 * implementations are behaviourally equivalent.
 *
 * Each test receives a factory function
 * `std::function<std::unique_ptr<FileSystem>(const
 * std::filesystem::path&)>`.  The fixture creates a unique temporary
 * directory for each test and tears it down afterwards.
 * `InMemoryFileSystem` ignores the temp-dir argument; `StdFileSystem`
 * operates on real files inside it.
 */

#include "raptor/interfaces/file_system.hpp"

#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "infrastructure/std_file_system.hpp"
#include "support/in_memory_file_system.hpp"

using Raptor::FileHandle;
using Raptor::FileSystem;
using Raptor::InMemoryFileSystem;
using Raptor::StdFileSystem;

// ---------------------------------------------------------------------------
// Factory type and fixture
// ---------------------------------------------------------------------------

/// Factory: given a temp-dir path, produce a ready `FileSystem`.
using FsFactory = std::function<std::unique_ptr<FileSystem>(
  const std::filesystem::path&)>;

/**
 * @brief Parametrized fixture providing a temporary directory and a
 *        `FileSystem` instance for each test.
 *
 * `SetUp` creates a unique temp directory; `TearDown` removes it
 * recursively.  `TestPath(name)` returns an absolute path inside the
 * temp dir — used by real-FS tests and harmlessly by the in-memory
 * implementation as a key string.
 */
class FileSystemTest : public ::testing::TestWithParam<FsFactory> {
 protected:
  void SetUp() override {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    static std::atomic<std::uint64_t> sCounter{0};
    mTempDir = std::filesystem::temp_directory_path() /
               ("raptor_fs_test_" + std::to_string(sCounter++));
    std::filesystem::create_directories(mTempDir);
    mFs = GetParam()(mTempDir);
  }

  void TearDown() override {
    mFs.reset();
    std::error_code ec;
    std::filesystem::remove_all(mTempDir, ec);
  }

  /// Returns an absolute path whose name is @p name inside the temp
  /// dir.
  [[nodiscard]] std::filesystem::path TestPath(
    std::string_view name) const {
    return mTempDir / name;
  }

  std::filesystem::path mTempDir;
  std::unique_ptr<FileSystem> mFs;
};

// ---------------------------------------------------------------------------
// Open and basic handle validity
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, OpenCreatesValidHandle) {
  auto result = mFs->Open(TestPath("a.dat"), 64);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->IsValid());
}

TEST_P(FileSystemTest, OpenWithZeroSizeSucceeds) {
  auto result = mFs->Open(TestPath("zero.dat"), 0);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->IsValid());
}

TEST_P(FileSystemTest, OpenSamePathTwiceSucceeds) {
  const auto path = TestPath("dup.dat");
  auto first = mFs->Open(path, 32);
  ASSERT_TRUE(first.has_value());
  auto second = mFs->Open(path, 32);
  ASSERT_TRUE(second.has_value());
}

// ---------------------------------------------------------------------------
// WriteAt / ReadAt round-trips
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, WriteAtThenReadBackSingleByte) {
  auto handle = mFs->Open(TestPath("b.dat"), 1);
  ASSERT_TRUE(handle.has_value());

  const std::array<std::byte, 1> written{std::byte{0xAB}};
  ASSERT_TRUE(mFs->WriteAt(*handle, 0, written).has_value());

  std::array<std::byte, 1> readBuf{};
  ASSERT_TRUE(mFs->ReadAt(*handle, 0, readBuf).has_value());
  EXPECT_EQ(readBuf[0], std::byte{0xAB});
}

TEST_P(FileSystemTest, WriteAtThenReadBackMultipleBytes) {
  constexpr std::size_t kSize = 8;
  auto handle = mFs->Open(TestPath("c.dat"), kSize);
  ASSERT_TRUE(handle.has_value());

  const std::array<std::byte, kSize> written{
    std::byte{0x01}, std::byte{0x02}, std::byte{0x03},
    std::byte{0x04}, std::byte{0x05}, std::byte{0x06},
    std::byte{0x07}, std::byte{0x08}};
  ASSERT_TRUE(mFs->WriteAt(*handle, 0, written).has_value());

  std::array<std::byte, kSize> readBuf{};
  ASSERT_TRUE(mFs->ReadAt(*handle, 0, readBuf).has_value());
  EXPECT_EQ(readBuf, written);
}

TEST_P(FileSystemTest, WriteAtNonZeroOffset) {
  constexpr std::size_t kSize = 16;
  auto handle = mFs->Open(TestPath("d.dat"), kSize);
  ASSERT_TRUE(handle.has_value());

  const std::array<std::byte, 4> written{
    std::byte{0xDE}, std::byte{0xAD}, std::byte{0xBE},
    std::byte{0xEF}};
  ASSERT_TRUE(mFs->WriteAt(*handle, 4, written).has_value());

  std::array<std::byte, 4> readBuf{};
  ASSERT_TRUE(mFs->ReadAt(*handle, 4, readBuf).has_value());
  EXPECT_EQ(readBuf, written);
}

TEST_P(FileSystemTest, MultipleDisjointWrites) {
  constexpr std::size_t kSize = 16;
  auto handle = mFs->Open(TestPath("e.dat"), kSize);
  ASSERT_TRUE(handle.has_value());

  const std::array<std::byte, 4> first{
    std::byte{0x11}, std::byte{0x22}, std::byte{0x33},
    std::byte{0x44}};
  const std::array<std::byte, 4> second{
    std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC},
    std::byte{0xDD}};

  ASSERT_TRUE(mFs->WriteAt(*handle, 0, first).has_value());
  ASSERT_TRUE(mFs->WriteAt(*handle, 8, second).has_value());

  std::array<std::byte, 4> readBuf{};
  ASSERT_TRUE(mFs->ReadAt(*handle, 0, readBuf).has_value());
  EXPECT_EQ(readBuf, first);

  ASSERT_TRUE(mFs->ReadAt(*handle, 8, readBuf).has_value());
  EXPECT_EQ(readBuf, second);
}

TEST_P(FileSystemTest, WriteAtEmptySpanSucceeds) {
  auto handle = mFs->Open(TestPath("empty_write.dat"), 8);
  ASSERT_TRUE(handle.has_value());
  const std::span<const std::byte> emptySpan;
  EXPECT_TRUE(mFs->WriteAt(*handle, 0, emptySpan).has_value());
}

TEST_P(FileSystemTest, ReadAtEmptySpanSucceeds) {
  auto handle = mFs->Open(TestPath("empty_read.dat"), 8);
  ASSERT_TRUE(handle.has_value());
  const std::span<std::byte> emptySpan;
  EXPECT_TRUE(mFs->ReadAt(*handle, 0, emptySpan).has_value());
}

// ---------------------------------------------------------------------------
// ReadAt error cases
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, ReadAtBeyondEndReturnsError) {
  auto handle = mFs->Open(TestPath("small.dat"), 4);
  ASSERT_TRUE(handle.has_value());

  std::array<std::byte, 8> readBuf{};
  const auto result = mFs->ReadAt(*handle, 0, readBuf);
  EXPECT_FALSE(result.has_value());
}

TEST_P(FileSystemTest, ReadAtOffsetBeyondEndReturnsError) {
  auto handle = mFs->Open(TestPath("small2.dat"), 4);
  ASSERT_TRUE(handle.has_value());

  std::array<std::byte, 1> readBuf{};
  const auto result = mFs->ReadAt(*handle, 10, readBuf);
  EXPECT_FALSE(result.has_value());
}

// ---------------------------------------------------------------------------
// FileSize
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, FileSizeMatchesPreallocation) {
  const auto path = TestPath("sized.dat");
  auto handle = mFs->Open(path, 256);
  ASSERT_TRUE(handle.has_value());

  const auto sizeResult = mFs->FileSize(path);
  ASSERT_TRUE(sizeResult.has_value());
  EXPECT_EQ(*sizeResult, 256U);
}

TEST_P(FileSystemTest, FileSizeGrowsOnWrite) {
  const auto path = TestPath("grow.dat");
  auto handle = mFs->Open(path, 0);
  ASSERT_TRUE(handle.has_value());

  const std::array<std::byte, 16> data{};
  ASSERT_TRUE(mFs->WriteAt(*handle, 0, data).has_value());

  const auto sizeResult = mFs->FileSize(path);
  ASSERT_TRUE(sizeResult.has_value());
  EXPECT_GE(*sizeResult, 16U);
}

TEST_P(FileSystemTest, FileSizeForMissingPathReturnsError) {
  const auto result = mFs->FileSize(TestPath("no_such_file.dat"));
  EXPECT_FALSE(result.has_value());
}

// ---------------------------------------------------------------------------
// Exists
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, ExistsReturnsTrueAfterOpen) {
  const auto path = TestPath("exists.dat");
  auto handle = mFs->Open(path, 8);
  ASSERT_TRUE(handle.has_value());

  const auto exists = mFs->Exists(path);
  ASSERT_TRUE(exists.has_value());
  EXPECT_TRUE(*exists);
}

TEST_P(FileSystemTest, ExistsReturnsFalseForUnknownPath) {
  const auto exists = mFs->Exists(TestPath("ghost.dat"));
  ASSERT_TRUE(exists.has_value());
  EXPECT_FALSE(*exists);
}

// ---------------------------------------------------------------------------
// Rename
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, RenameMovesFile) {
  const auto from = TestPath("old.dat");
  const auto to = TestPath("new.dat");

  {
    auto handle = mFs->Open(from, 8);
    ASSERT_TRUE(handle.has_value());
  }  // handle destroyed here — file closed before rename

  ASSERT_TRUE(mFs->Rename(from, to).has_value());

  const auto fromExists = mFs->Exists(from);
  ASSERT_TRUE(fromExists.has_value());
  EXPECT_FALSE(*fromExists);

  const auto toExists = mFs->Exists(to);
  ASSERT_TRUE(toExists.has_value());
  EXPECT_TRUE(*toExists);
}

TEST_P(FileSystemTest, RenamePreservesContent) {
  const auto from = TestPath("src.dat");
  const auto to = TestPath("dst.dat");

  {
    auto handle = mFs->Open(from, 4);
    ASSERT_TRUE(handle.has_value());
    const std::array<std::byte, 4> written{
      std::byte{0xCA}, std::byte{0xFE}, std::byte{0xBA},
      std::byte{0xBE}};
    ASSERT_TRUE(mFs->WriteAt(*handle, 0, written).has_value());
  }  // handle goes out of scope (closed)

  ASSERT_TRUE(mFs->Rename(from, to).has_value());

  auto handle = mFs->Open(to, 4);
  ASSERT_TRUE(handle.has_value());

  std::array<std::byte, 4> readBuf{};
  ASSERT_TRUE(mFs->ReadAt(*handle, 0, readBuf).has_value());

  const std::array<std::byte, 4> expected{
    std::byte{0xCA}, std::byte{0xFE}, std::byte{0xBA},
    std::byte{0xBE}};
  EXPECT_EQ(readBuf, expected);
}

TEST_P(FileSystemTest, RenameMissingFileReturnsError) {
  const auto result =
    mFs->Rename(TestPath("missing.dat"), TestPath("target.dat"));
  EXPECT_FALSE(result.has_value());
}

// ---------------------------------------------------------------------------
// Remove
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, RemoveDeletesFile) {
  const auto path = TestPath("todel.dat");
  {
    auto handle = mFs->Open(path, 8);
    ASSERT_TRUE(handle.has_value());
  }  // handle destroyed — file closed before remove

  ASSERT_TRUE(mFs->Remove(path).has_value());

  const auto exists = mFs->Exists(path);
  ASSERT_TRUE(exists.has_value());
  EXPECT_FALSE(*exists);
}

TEST_P(FileSystemTest, RemoveMissingPathSucceeds) {
  EXPECT_TRUE(mFs->Remove(TestPath("never_existed.dat")).has_value());
}

// ---------------------------------------------------------------------------
// CreateDirectories
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, CreateDirectoriesSucceeds) {
  EXPECT_TRUE(
    mFs->CreateDirectories(TestPath("sub/dir")).has_value());
}

// ---------------------------------------------------------------------------
// FileHandle move semantics
// ---------------------------------------------------------------------------

TEST_P(FileSystemTest, MoveConstructedHandleRemainsValid) {
  auto handle = mFs->Open(TestPath("mv.dat"), 4);
  ASSERT_TRUE(handle.has_value());

  FileHandle moved = std::move(*handle);
  EXPECT_TRUE(moved.IsValid());
  EXPECT_FALSE(handle->IsValid());  // NOLINT(bugprone-use-after-move)
}

TEST_P(FileSystemTest, MoveAssignedHandleRemainsValid) {
  auto handleA = mFs->Open(TestPath("mva.dat"), 4);
  auto handleB = mFs->Open(TestPath("mvb.dat"), 4);
  ASSERT_TRUE(handleA.has_value());
  ASSERT_TRUE(handleB.has_value());

  *handleA = std::move(*handleB);
  EXPECT_TRUE(handleA->IsValid());
  EXPECT_FALSE(
    handleB->IsValid());  // NOLINT(bugprone-use-after-move)
}

// ---------------------------------------------------------------------------
// Parametrize over both implementations
// ---------------------------------------------------------------------------

// NOLINTNEXTLINE(cert-err58-cpp)
INSTANTIATE_TEST_SUITE_P(
  InMemory, FileSystemTest,
  ::testing::Values(FsFactory{[](const std::filesystem::path&) {
    return std::make_unique<InMemoryFileSystem>();
  }}),
  [](const ::testing::TestParamInfo<FsFactory>&) {
    return std::string{"InMemoryFileSystem"};
  });

// NOLINTNEXTLINE(cert-err58-cpp)
INSTANTIATE_TEST_SUITE_P(
  Std, FileSystemTest,
  ::testing::Values(FsFactory{[](const std::filesystem::path&) {
    return std::make_unique<StdFileSystem>();
  }}),
  [](const ::testing::TestParamInfo<FsFactory>&) {
    return std::string{"StdFileSystem"};
  });

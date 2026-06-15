/**
 * @file in_memory_file_system.hpp
 * @brief In-memory `FileSystem` test double for Raptor unit tests.
 *
 * `InMemoryFileSystem` mirrors the behaviour of `StdFileSystem` using
 * `std::vector<std::byte>` buffers stored in an `std::unordered_map`
 * keyed on the path string.  No files are created on disk; tests run
 * fully offline and deterministically.
 *
 * ## Usage
 *
 * @code
 * InMemoryFileSystem fs;
 *
 * auto handleResult = fs.Open("piece.dat", 1024);
 * ASSERT_TRUE(handleResult.has_value());
 *
 * std::array<std::byte, 4> buf{std::byte{0xAB}};
 * ASSERT_TRUE(fs.WriteAt(*handleResult, 0, buf).has_value());
 * @endcode
 */

#ifndef RAPTOR_TESTS_SUPPORT_IN_MEMORY_FILE_SYSTEM_HPP
#define RAPTOR_TESTS_SUPPORT_IN_MEMORY_FILE_SYSTEM_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "raptor/interfaces/file_system.hpp"

namespace Raptor {

/**
 * @brief Fully in-memory `FileSystem` implementation for tests.
 *
 * File "paths" are used as opaque string keys — no real directory
 * lookup is performed.  `CreateDirectories` and `Remove` are
 * supported for interface parity.
 *
 * ### Open semantics
 *
 * - If the path does not exist in the internal store, a zero-filled
 *   buffer of @p size bytes is created.
 * - If the path already exists and its buffer is smaller than
 *   @p size, the buffer is grown with zero bytes.
 * - If the path already exists and is at least @p size bytes, the
 *   buffer is left unchanged.
 *
 * ### Handle internals
 *
 * The `FileHandle::NativeHandle` is a `std::uint64_t` open-file ID
 * used to look up the path in `mOpenFiles`.  The closer erases the
 * entry from `mOpenFiles` on handle destruction.
 *
 * @note Not thread-safe.  Intended for single-threaded test fixtures
 *       only.
 */
class InMemoryFileSystem final : public FileSystem {
 public:
  InMemoryFileSystem() = default;

  /**
   * @brief Opens (or creates) an in-memory file of @p size bytes.
   *
   * @param path  Path used as the storage key.
   * @param size  Desired minimum file size.
   * @return A valid `FileHandle` on success.
   */
  [[nodiscard]] Result<FileHandle> Open(
    const std::filesystem::path& path, std::uint64_t size) override;

  /**
   * @brief Writes @p data into the in-memory buffer at @p offset.
   *
   * The buffer is automatically grown if the write extends beyond the
   * current end.
   *
   * @param handle  Valid handle from `Open`.
   * @param offset  Byte offset from the start of the buffer.
   * @param data    Bytes to write.
   * @return `Status{}` on success, or `Errc::Io` for an invalid
   *         handle.
   */
  [[nodiscard]] Status WriteAt(
    const FileHandle& handle, std::uint64_t offset,
    std::span<const std::byte> data) override;

  /**
   * @brief Reads exactly `buffer.size()` bytes from the in-memory
   *        buffer at @p offset.
   *
   * Returns `Errc::Io` if @p offset + `buffer.size()` exceeds the
   * current buffer size (short read not supported).
   *
   * @param handle  Valid handle from `Open`.
   * @param offset  Byte offset from the start of the buffer.
   * @param buffer  Destination span.
   * @return `Status{}` on success.
   */
  [[nodiscard]] Status ReadAt(const FileHandle& handle,
                              std::uint64_t offset,
                              std::span<std::byte> buffer) override;

  /**
   * @brief Returns whether a buffer exists for @p path.
   *
   * @param path  Path key to query.
   * @return `true` if the path was previously opened, `false`
   *         otherwise.
   */
  [[nodiscard]] Result<bool> Exists(
    const std::filesystem::path& path) override;

  /**
   * @brief Returns the current size of the in-memory buffer for
   *        @p path.
   *
   * @param path  Path key to query.
   * @return Buffer size, or `Errc::Io` if the path is not known.
   */
  [[nodiscard]] Result<std::uint64_t> FileSize(
    const std::filesystem::path& path) override;

  /**
   * @brief Renames the buffer key from @p from to @p to.
   *
   * @param from  Existing path key.
   * @param to    New path key; must not already exist.
   * @return `Status{}` on success, or `Errc::Io` if @p from is
   *         unknown.
   */
  [[nodiscard]] Status Rename(
    const std::filesystem::path& from,
    const std::filesystem::path& to) override;

  /**
   * @brief Erases the buffer for @p path from the internal store.
   *
   * No-op if @p path is not known.
   *
   * @param path  Path key to remove.
   * @return Always `Status{}`.
   */
  [[nodiscard]] Status Remove(
    const std::filesystem::path& path) override;

  /**
   * @brief No-op; in-memory storage has no real directory structure.
   *
   * @param path  Ignored.
   * @return Always `Status{}`.
   */
  [[nodiscard]] Status CreateDirectories(
    const std::filesystem::path& path) override;

 private:
  /// File content keyed by canonical path string.
  std::unordered_map<std::string, std::vector<std::byte>> mFiles;

  /// Maps open-file ID → path string for active handles.
  std::unordered_map<std::uint64_t, std::string> mOpenFiles;

  /// Monotonically increasing handle ID counter.
  std::uint64_t mNextId{1};

  /**
   * @brief Looks up the path string for an open handle.
   *
   * @param handle  Handle whose path to retrieve.
   * @return Pointer to the path string, or nullptr if not found.
   */
  [[nodiscard]] const std::string* PathFor(
    const FileHandle& handle) const noexcept;
};

// ---------------------------------------------------------------------------
// Inline implementation (header-only for test support)
// ---------------------------------------------------------------------------

inline Result<FileHandle> InMemoryFileSystem::Open(
  const std::filesystem::path& path, std::uint64_t size) {
  const auto key = path.string();

  auto& buffer = mFiles[key];
  if (buffer.size() < static_cast<std::size_t>(size)) {
    buffer.resize(static_cast<std::size_t>(size), std::byte{0});
  }

  const std::uint64_t id = mNextId++;
  mOpenFiles[id] = key;

  auto closer = [this](FileHandle::NativeHandle h) noexcept {
    mOpenFiles.erase(h);
  };

  return FileHandle{id, std::move(closer)};
}

inline Status InMemoryFileSystem::WriteAt(
  const FileHandle& handle, std::uint64_t offset,
  std::span<const std::byte> data) {
  if (data.empty()) {
    return {};
  }
  const auto* pathPtr = PathFor(handle);
  if (pathPtr == nullptr) {
    return Fail(Errc::Io, "invalid file handle");
  }
  auto& buffer = mFiles[*pathPtr];
  const auto endPos = static_cast<std::size_t>(offset) + data.size();
  if (endPos > buffer.size()) {
    buffer.resize(endPos, std::byte{0});
  }
  std::copy(data.begin(), data.end(),
            buffer.begin() + static_cast<std::ptrdiff_t>(offset));
  return {};
}

inline Status InMemoryFileSystem::ReadAt(
  const FileHandle& handle, std::uint64_t offset,
  std::span<std::byte> buffer) {
  if (buffer.empty()) {
    return {};
  }
  const auto* pathPtr = PathFor(handle);
  if (pathPtr == nullptr) {
    return Fail(Errc::Io, "invalid file handle");
  }
  const auto fileIt = mFiles.find(*pathPtr);
  if (fileIt == mFiles.end()) {
    return Fail(Errc::Io, "file data not found for handle");
  }
  const auto& fileData = fileIt->second;
  const auto startPos = static_cast<std::size_t>(offset);
  const auto endPos = startPos + buffer.size();
  if (endPos > fileData.size()) {
    return Fail(Errc::Io, "read beyond end of file");
  }
  std::copy(fileData.begin() + static_cast<std::ptrdiff_t>(startPos),
            fileData.begin() + static_cast<std::ptrdiff_t>(endPos),
            buffer.begin());
  return {};
}

inline Result<bool> InMemoryFileSystem::Exists(
  const std::filesystem::path& path) {
  return mFiles.contains(path.string());
}

inline Result<std::uint64_t> InMemoryFileSystem::FileSize(
  const std::filesystem::path& path) {
  const auto it = mFiles.find(path.string());
  if (it == mFiles.end()) {
    return Fail(Errc::Io, "file not found: " + path.string());
  }
  return static_cast<std::uint64_t>(it->second.size());
}

inline Status InMemoryFileSystem::Rename(
  const std::filesystem::path& from,
  const std::filesystem::path& to) {
  const auto fromKey = from.string();
  const auto toKey = to.string();
  auto it = mFiles.find(fromKey);
  if (it == mFiles.end()) {
    return Fail(Errc::Io, "file not found: " + fromKey);
  }
  mFiles[toKey] = std::move(it->second);
  mFiles.erase(it);
  return {};
}

inline Status InMemoryFileSystem::Remove(
  const std::filesystem::path& path) {
  mFiles.erase(path.string());
  return {};
}

inline Status InMemoryFileSystem::CreateDirectories(
  const std::filesystem::path& /*path*/) {
  return {};
}

inline const std::string* InMemoryFileSystem::PathFor(
  const FileHandle& handle) const noexcept {
  const auto it = mOpenFiles.find(handle.Native());
  if (it == mOpenFiles.end()) {
    return nullptr;
  }
  return &it->second;
}

}  // namespace Raptor

#endif  // RAPTOR_TESTS_SUPPORT_IN_MEMORY_FILE_SYSTEM_HPP

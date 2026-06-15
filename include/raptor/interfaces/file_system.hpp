/**
 * @file file_system.hpp
 * @brief Abstract filesystem interface for Raptor.
 *
 * All disk access in Raptor goes through `FileSystem`.  This keeps
 * piece-writing, resume-state, and metainfo-loading logic fully
 * decoupled from the OS so tests can use `InMemoryFileSystem` without
 * touching real files.
 *
 * ## Design
 *
 * - `FileSystem` is a pure-virtual interface; concrete adapters live
 *   in `infrastructure/`.
 * - `FileHandle` (see `raptor/interfaces/file_handle.hpp`) is a
 *   move-only RAII wrapper around an opaque native handle.
 * Destruction automatically releases OS resources.
 * - All fallible operations return `Result<T>` or `Status`; callers
 *   must inspect the value.
 *
 * ## Usage
 *
 * @code
 * Result<FileHandle> result = fs.Open(path, fileSize);
 * if (!result) { return result.error(); }
 *
 * FileHandle handle = std::move(*result);
 * Status writeStatus = fs.WriteAt(handle, 0, data);
 * @endcode
 */

#ifndef RAPTOR_INTERFACES_FILE_SYSTEM_HPP
#define RAPTOR_INTERFACES_FILE_SYSTEM_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>

#include "raptor/domain/error.hpp"
#include "raptor/interfaces/file_handle.hpp"

namespace Raptor {

/**
 * @brief Abstract interface for all Raptor disk operations.
 *
 * Raptor's domain and application layers depend only on this
 * interface, never on OS headers or `<filesystem>` internals.
 * Production code uses `StdFileSystem`; tests use
 * `InMemoryFileSystem`.
 *
 * ## Offset semantics
 *
 * All read/write operations use an explicit byte offset from the
 * start of the file.  The interface provides no implicit seek
 * position; each call is independent.
 *
 * ## Error reporting
 *
 * Expected failures (missing files, I/O errors, out-of-range
 * offsets) are returned as `Status` or `Result<T>` with
 * `Errc::Io`.  Programmer errors (null handle, invalid arguments)
 * may be expressed as assertions.
 */
class FileSystem {
 public:
  virtual ~FileSystem() = default;

  FileSystem() = default;
  FileSystem(const FileSystem&) = delete;
  FileSystem& operator=(const FileSystem&) = delete;
  FileSystem(FileSystem&&) = delete;
  FileSystem& operator=(FileSystem&&) = delete;

  /**
   * @brief Opens (or creates) a file, preallocating @p size bytes.
   *
   * If the file does not exist it is created and extended to @p size
   * bytes (zero-filled or sparse, depending on the implementation).
   * If it already exists, it is opened for random read/write access;
   * its size is grown to @p size if smaller, but never shrunk.
   *
   * @param path  Absolute or relative path to the target file.
   * @param size  Desired file size in bytes.  Pass 0 to open
   *              without preallocation.
   * @return A valid `FileHandle` on success, or an `Error` with
   *         `Errc::Io` on failure.
   */
  [[nodiscard]] virtual Result<FileHandle> Open(
    const std::filesystem::path& path, std::uint64_t size) = 0;

  /**
   * @brief Writes @p data to @p handle starting at @p offset.
   *
   * The write is always the full length of @p data; a short write is
   * treated as an error.
   *
   * @param handle  Open file handle obtained from `Open`.
   * @param offset  Byte offset from the start of the file.
   * @param data    Bytes to write.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] virtual Status WriteAt(
    const FileHandle& handle, std::uint64_t offset,
    std::span<const std::byte> data) = 0;

  /**
   * @brief Reads exactly `buffer.size()` bytes from @p handle at
   *        @p offset into @p buffer.
   *
   * A short read (e.g. offset + length exceeds file size) is treated
   * as an error.
   *
   * @param handle  Open file handle obtained from `Open`.
   * @param offset  Byte offset from the start of the file.
   * @param buffer  Destination buffer; must be non-empty.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] virtual Status ReadAt(
    const FileHandle& handle, std::uint64_t offset,
    std::span<std::byte> buffer) = 0;

  /**
   * @brief Returns true if @p path names an existing regular file or
   *        directory.
   *
   * @param path  Path to query.
   * @return `true` if the path exists, `false` if it does not.
   *         Returns `Errc::Io` if the existence check itself fails
   *         (e.g. permission denied).
   */
  [[nodiscard]] virtual Result<bool> Exists(
    const std::filesystem::path& path) = 0;

  /**
   * @brief Returns the size of the file at @p path in bytes.
   *
   * @param path  Path to query; must name an existing regular file.
   * @return File size on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] virtual Result<std::uint64_t> FileSize(
    const std::filesystem::path& path) = 0;

  /**
   * @brief Renames (moves) a file from @p from to @p to.
   *
   * Overwrites @p to if it already exists.
   *
   * @param from  Existing path.
   * @param to    Target path; parent directory must exist.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] virtual Status Rename(
    const std::filesystem::path& from,
    const std::filesystem::path& to) = 0;

  /**
   * @brief Removes the file or empty directory at @p path.
   *
   * Succeeds silently if @p path does not exist.
   *
   * @param path  Target to remove.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] virtual Status Remove(
    const std::filesystem::path& path) = 0;

  /**
   * @brief Creates @p path and all missing parent directories.
   *
   * Succeeds silently if the directory already exists.
   *
   * @param path  Directory path to create.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] virtual Status CreateDirectories(
    const std::filesystem::path& path) = 0;
};

}  // namespace Raptor

#endif  // RAPTOR_INTERFACES_FILE_SYSTEM_HPP

/**
 * @file std_file_system.hpp
 * @brief Production `FileSystem` adapter backed by `std::fstream` and
 *        `std::filesystem`.
 *
 * `StdFileSystem` is the concrete implementation used in the running
 * Raptor binary.  It delegates to:
 *
 * - `std::filesystem` for path operations (exists, size, rename,
 *   remove, create_directories).
 * - `std::fstream` (opened in binary random-access mode) for
 *   positional read/write via `seekg`/`seekp`.
 *
 * ## Dependency note
 *
 * This header lives in `src/infrastructure/` and must never be
 * included from `include/raptor/` domain or interface headers.
 * Application code depends only on
 * `raptor/interfaces/file_system.hpp`.
 */

#ifndef RAPTOR_INFRASTRUCTURE_STD_FILE_SYSTEM_HPP
#define RAPTOR_INFRASTRUCTURE_STD_FILE_SYSTEM_HPP

#include "raptor/interfaces/file_system.hpp"

namespace Raptor {

/**
 * @brief `FileSystem` implementation that performs real disk I/O.
 *
 * `Open` creates missing files and preallocates them to the requested
 * size using `std::filesystem::resize_file`.  Existing files are
 * opened for random read/write access and grown if smaller than the
 * requested size.
 *
 * Each `FileHandle` returned by `Open` owns a heap-allocated
 * `std::fstream`; the stream is closed and deallocated when the
 * handle is destroyed.
 */
class StdFileSystem final : public FileSystem {
 public:
  StdFileSystem() = default;

  /**
   * @brief Opens or creates @p path, preallocating @p size bytes.
   *
   * If the file does not exist it is created with `std::ios::trunc`
   * and then extended to @p size via `std::filesystem::resize_file`.
   * If it already exists it is opened in read/write binary mode; the
   * file is extended to @p size if its current size is smaller.
   *
   * @param path  Target file path.
   * @param size  Desired file size in bytes.
   * @return A valid `FileHandle` on success, or `Errc::Io` on
   *         failure.
   */
  [[nodiscard]] Result<FileHandle> Open(
    const std::filesystem::path& path, std::uint64_t size) override;

  /**
   * @brief Writes @p data to the file at @p offset.
   *
   * Seeks to @p offset then writes all bytes in @p data.  A seek or
   * write failure yields `Errc::Io`.
   *
   * @param handle  Valid handle from `Open`.
   * @param offset  Byte offset from file start.
   * @param data    Bytes to write.
   * @return `Status{}` on success.
   */
  [[nodiscard]] Status WriteAt(
    const FileHandle& handle, std::uint64_t offset,
    std::span<const std::byte> data) override;

  /**
   * @brief Reads exactly `buffer.size()` bytes from the file at
   *        @p offset.
   *
   * Seeks to @p offset then reads into @p buffer.  A seek, short
   * read, or read failure yields `Errc::Io`.
   *
   * @param handle  Valid handle from `Open`.
   * @param offset  Byte offset from file start.
   * @param buffer  Destination buffer.
   * @return `Status{}` on success.
   */
  [[nodiscard]] Status ReadAt(const FileHandle& handle,
                              std::uint64_t offset,
                              std::span<std::byte> buffer) override;

  /**
   * @brief Returns whether @p path exists on the real filesystem.
   *
   * @param path  Path to query.
   * @return `true` / `false`, or `Errc::Io` if the check throws.
   */
  [[nodiscard]] Result<bool> Exists(
    const std::filesystem::path& path) override;

  /**
   * @brief Returns the size of the file at @p path.
   *
   * @param path  Regular file path.
   * @return Size in bytes, or `Errc::Io` on failure.
   */
  [[nodiscard]] Result<std::uint64_t> FileSize(
    const std::filesystem::path& path) override;

  /**
   * @brief Renames (atomically on most POSIX systems) @p from to
   *        @p to.
   *
   * @param from  Source path.
   * @param to    Destination path.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] Status Rename(
    const std::filesystem::path& from,
    const std::filesystem::path& to) override;

  /**
   * @brief Removes the file or empty directory at @p path.
   *
   * No-op if @p path does not exist.
   *
   * @param path  Target path.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] Status Remove(
    const std::filesystem::path& path) override;

  /**
   * @brief Creates @p path and any missing parent directories.
   *
   * No-op if the directory already exists.
   *
   * @param path  Directory path to create.
   * @return `Status{}` on success, or `Errc::Io` on failure.
   */
  [[nodiscard]] Status CreateDirectories(
    const std::filesystem::path& path) override;
};

}  // namespace Raptor

#endif  // RAPTOR_INFRASTRUCTURE_STD_FILE_SYSTEM_HPP

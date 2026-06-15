/**
 * @file file_handle.hpp
 * @brief RAII handle to an open file resource.
 *
 * `FileHandle` is the move-only token returned by `FileSystem::Open`.
 * It owns the lifetime of whatever OS or in-memory resource backs the
 * open file; destruction (or move-assignment) releases that resource
 * via the closer supplied at construction time.
 *
 * ## Usage
 *
 * @code
 * Result<FileHandle> result = fs.Open(path, size);
 * if (!result) { return result.error(); }
 *
 * FileHandle handle = std::move(*result);
 * // ... pass handle to fs.WriteAt / fs.ReadAt ...
 * // handle released automatically when it goes out of scope
 * @endcode
 */

#ifndef RAPTOR_INTERFACES_FILE_HANDLE_HPP
#define RAPTOR_INTERFACES_FILE_HANDLE_HPP

#include <cstdint>
#include <functional>

namespace Raptor {

/**
 * @brief Move-only RAII wrapper around an open-file resource.
 *
 * `FileHandle` owns the lifetime of an open file.  When it is
 * destroyed (or move-assigned over), the associated OS handle is
 * released via the closer function supplied at construction.
 *
 * Concrete `FileSystem` implementations create `FileHandle` instances
 * through the two-argument constructor.  Callers treat handles as
 * opaque tokens and pass them back to `FileSystem` methods.
 *
 * @note Not thread-safe.  Do not share a `FileHandle` across threads
 *       without external synchronisation.
 */
class FileHandle {
 public:
  /// Opaque integer representation of the underlying OS handle.
  using NativeHandle = std::uintptr_t;

  /// @brief Constructs an invalid (empty) handle.
  FileHandle() = default;

  /**
   * @brief Constructs a valid handle from a native value and a
   * closer.
   *
   * @param nativeHandle  Opaque value identifying the open resource
   *                      (e.g. a FILE* cast to uintptr_t, or an index
   *                      into an in-memory store).
   * @param closer        Callable invoked with @p nativeHandle on
   *                      destruction or move-assignment; must not
   * throw.
   */
  explicit FileHandle(
    NativeHandle nativeHandle,
    std::function<void(NativeHandle)> closer) noexcept
      : mNativeHandle{nativeHandle}, mCloser{std::move(closer)} {
  }

  /// @brief Releases the underlying resource via the stored closer.
  ~FileHandle() {
    if (mCloser) {
      mCloser(mNativeHandle);
    }
  }

  FileHandle(const FileHandle&) = delete;
  FileHandle& operator=(const FileHandle&) = delete;

  /**
   * @brief Move-constructs from @p other; @p other becomes invalid.
   *
   * @param other  Source handle; its closer is transferred and it is
   *               left in the invalid (empty) state.
   */
  FileHandle(FileHandle&& other) noexcept
      : mNativeHandle{other.mNativeHandle},
        mCloser{std::move(other.mCloser)} {
    other.mNativeHandle = 0;
    // std::function move leaves the source in a valid-but-unspecified
    // state; explicitly null it so IsValid() reliably returns false.
    other.mCloser = nullptr;
  }

  /**
   * @brief Move-assigns from @p other; the current resource is
   *        released first.
   *
   * Self-assignment is a no-op.
   *
   * @param other  Source handle; transferred and left invalid.
   * @return Reference to *this.
   */
  FileHandle& operator=(FileHandle&& other) noexcept {
    if (this != &other) {
      if (mCloser) {
        mCloser(mNativeHandle);
      }
      mNativeHandle = other.mNativeHandle;
      mCloser = std::move(other.mCloser);
      other.mNativeHandle = 0;
      other.mCloser =
        nullptr;  // ensure moved-from IsValid() == false
    }
    return *this;
  }

  /**
   * @brief Returns true if the handle refers to an open resource.
   *
   * A default-constructed or moved-from handle is invalid.
   */
  [[nodiscard]] bool IsValid() const noexcept {
    return static_cast<bool>(mCloser);
  }

  /**
   * @brief Returns the opaque native handle value.
   *
   * Intended for use by `FileSystem` implementations only.  Do not
   * interpret this value in application or domain code.
   *
   * @return The native handle supplied at construction.
   */
  [[nodiscard]] NativeHandle Native() const noexcept {
    return mNativeHandle;
  }

 private:
  NativeHandle mNativeHandle{0};
  std::function<void(NativeHandle)> mCloser;
};

}  // namespace Raptor

#endif  // RAPTOR_INTERFACES_FILE_HANDLE_HPP

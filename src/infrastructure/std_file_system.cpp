/**
 * @file std_file_system.cpp
 * @brief Implementation of `StdFileSystem` — the production
 *        `FileSystem` adapter.
 */

#include "std_file_system.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <span>
#include <string>
#include <system_error>

namespace Raptor {

namespace {

/**
 * @brief Casts a `FileHandle::NativeHandle` back to the owning
 *        `std::fstream*`.
 *
 * The pointer was stored as an opaque `uintptr_t` in `Open`.  The
 * reinterpret_cast here is the inverse of the one used at creation
 * time and is safe as long as the handle is valid.
 *
 * @param native  Opaque native handle value.
 * @return Non-owning pointer to the underlying stream.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
[[nodiscard]] inline std::fstream* StreamFrom(
  FileHandle::NativeHandle native) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<std::fstream*>(native);
}

}  // namespace

// ---------------------------------------------------------------------------
// Open
// ---------------------------------------------------------------------------

Result<FileHandle> StdFileSystem::Open(
  const std::filesystem::path& path, std::uint64_t size) {
  std::error_code ec;

  const bool alreadyExists = std::filesystem::exists(path, ec);
  if (ec) {
    return Fail(Errc::Io, "exists check failed: " + ec.message());
  }

  // Choose open flags: truncate-create for new files, r/w for
  // existing.
  const auto createFlags =
    std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
  const auto openFlags =
    std::ios::in | std::ios::out | std::ios::binary;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  auto* stream =
    new std::fstream(path, alreadyExists ? openFlags : createFlags);
  if (!stream->is_open()) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete stream;
    return Fail(Errc::Io, "cannot open file: " + path.string());
  }

  // Preallocate / grow the file to the requested size.
  if (size > 0) {
    const auto currentSize = std::filesystem::file_size(path, ec);
    if (ec) {
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      delete stream;
      return Fail(Errc::Io, "file_size failed: " + ec.message());
    }
    if (currentSize < size) {
      std::filesystem::resize_file(path, size, ec);
      if (ec) {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        delete stream;
        return Fail(Errc::Io, "resize_file failed: " + ec.message());
      }
    }
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto nativeHandle =
    reinterpret_cast<FileHandle::NativeHandle>(stream);

  auto closer = [](FileHandle::NativeHandle h) noexcept {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-owning-memory)
    delete reinterpret_cast<std::fstream*>(h);
  };

  return FileHandle{nativeHandle, std::move(closer)};
}

// ---------------------------------------------------------------------------
// WriteAt
// ---------------------------------------------------------------------------

Status StdFileSystem::WriteAt(const FileHandle& handle,
                              std::uint64_t offset,
                              std::span<const std::byte> data) {
  if (data.empty()) {
    return {};
  }

  auto* stream = StreamFrom(handle.Native());

  stream->seekp(static_cast<std::streamoff>(offset));
  if (!stream->good()) {
    return Fail(Errc::Io,
                "seekp failed at offset " + std::to_string(offset));
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  stream->write(reinterpret_cast<const char*>(data.data()),
                static_cast<std::streamsize>(data.size()));

  if (!stream->good()) {
    return Fail(Errc::Io, "write failed");
  }

  stream->flush();
  return {};
}

// ---------------------------------------------------------------------------
// ReadAt
// ---------------------------------------------------------------------------

Status StdFileSystem::ReadAt(const FileHandle& handle,
                             std::uint64_t offset,
                             std::span<std::byte> buffer) {
  if (buffer.empty()) {
    return {};
  }

  auto* stream = StreamFrom(handle.Native());

  stream->seekg(static_cast<std::streamoff>(offset));
  if (!stream->good()) {
    return Fail(Errc::Io,
                "seekg failed at offset " + std::to_string(offset));
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  stream->read(reinterpret_cast<char*>(buffer.data()),
               static_cast<std::streamsize>(buffer.size()));

  if (stream->fail()) {
    return Fail(Errc::Io, "read failed or short read");
  }

  return {};
}

// ---------------------------------------------------------------------------
// Exists
// ---------------------------------------------------------------------------

Result<bool> StdFileSystem::Exists(
  const std::filesystem::path& path) {
  std::error_code ec;
  const bool result = std::filesystem::exists(path, ec);
  if (ec) {
    return Fail(Errc::Io, "exists check failed: " + ec.message());
  }
  return result;
}

// ---------------------------------------------------------------------------
// FileSize
// ---------------------------------------------------------------------------

Result<std::uint64_t> StdFileSystem::FileSize(
  const std::filesystem::path& path) {
  std::error_code ec;
  const auto size = std::filesystem::file_size(path, ec);
  if (ec) {
    return Fail(Errc::Io, "file_size failed: " + ec.message());
  }
  return static_cast<std::uint64_t>(size);
}

// ---------------------------------------------------------------------------
// Rename
// ---------------------------------------------------------------------------

Status StdFileSystem::Rename(const std::filesystem::path& from,
                             const std::filesystem::path& to) {
  std::error_code ec;
  std::filesystem::rename(from, to, ec);
  if (ec) {
    return Fail(Errc::Io, "rename failed: " + ec.message());
  }
  return {};
}

// ---------------------------------------------------------------------------
// Remove
// ---------------------------------------------------------------------------

Status StdFileSystem::Remove(const std::filesystem::path& path) {
  std::error_code ec;
  std::filesystem::remove(path, ec);
  if (ec) {
    return Fail(Errc::Io, "remove failed: " + ec.message());
  }
  return {};
}

// ---------------------------------------------------------------------------
// CreateDirectories
// ---------------------------------------------------------------------------

Status StdFileSystem::CreateDirectories(
  const std::filesystem::path& path) {
  std::error_code ec;
  std::filesystem::create_directories(path, ec);
  if (ec) {
    return Fail(Errc::Io,
                "create_directories failed: " + ec.message());
  }
  return {};
}

}  // namespace Raptor

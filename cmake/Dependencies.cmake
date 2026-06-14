# =============================================================================
# Dependencies.cmake — third-party libraries via CMake FetchContent
#
# All dependencies are pinned to immutable git tags so every developer and CI
# job resolves exactly the same versions.  Setting FETCHCONTENT_UPDATES_DISCONNECTED
# avoids redundant network round-trips on reconfigure once the source cache is warm.
#
# Dependency wiring rules (mirrors the architecture layering rules):
#   - Only infrastructure/ targets may link these libraries directly.
#   - domain/ must stay third-party-free.
# =============================================================================

include(FetchContent)

# Avoid redundant update checks when the source cache already exists.
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# -----------------------------------------------------------------------------
# spdlog — structured, fast logging (infrastructure adapter only)
# -----------------------------------------------------------------------------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.3
    GIT_SHALLOW    ON
    SYSTEM         # suppress warnings from third-party headers
)
FetchContent_MakeAvailable(spdlog)

# -----------------------------------------------------------------------------
# asio — standalone, header-only async I/O runtime (task 0020)
# Using the standalone (non-Boost) flavour; no separate CMake target is built,
# so we create an INTERFACE target manually after population.
# -----------------------------------------------------------------------------
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-34-2
    GIT_SHALLOW    ON
    SYSTEM
)
FetchContent_MakeAvailable(asio)

# asio ships no CMakeLists.txt — expose include path via an INTERFACE target.
if(NOT TARGET asio::asio)
    add_library(asio::asio INTERFACE IMPORTED GLOBAL)
    target_include_directories(asio::asio SYSTEM INTERFACE
        "${asio_SOURCE_DIR}/asio/include"
    )
    # Standalone asio: disable Boost integration and deprecated features.
    target_compile_definitions(asio::asio INTERFACE
        ASIO_STANDALONE
        ASIO_NO_DEPRECATED
    )
endif()

# -----------------------------------------------------------------------------
# CLI11 — command-line argument parsing (task 0053)
# -----------------------------------------------------------------------------
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.5.0
    GIT_SHALLOW    ON
    SYSTEM
)
FetchContent_MakeAvailable(CLI11)

# -----------------------------------------------------------------------------
# googletest (GTest + GMock) — unit-testing framework
# Only fetched and compiled when tests are enabled.
# -----------------------------------------------------------------------------
if(RAPTOR_BUILD_TESTS)
    # Prevent GoogleTest from overriding our compiler/linker settings.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
        GIT_SHALLOW    ON
        SYSTEM
    )
    FetchContent_MakeAvailable(googletest)
endif()

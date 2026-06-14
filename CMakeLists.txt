cmake_minimum_required(VERSION 3.28)
project(raptor CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
include(cmake/Warnings.cmake)

# ---------------------------------------------------------------------------
# Third-party dependencies (FetchContent, pinned tags)
# RAPTOR_BUILD_TESTS defaults ON for top-level builds, OFF as a subproject.
# ---------------------------------------------------------------------------
option(RAPTOR_BUILD_TESTS "Build unit tests (fetches googletest)" ${PROJECT_IS_TOP_LEVEL})
include(cmake/Dependencies.cmake)

# ---------------------------------------------------------------------------
# Optional sanitizers (enabled via RAPTOR_SANITIZE=ON, e.g. the "dev" preset)
# ---------------------------------------------------------------------------
option(RAPTOR_SANITIZE "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(RAPTOR_SANITIZE)
    if(MSVC)
        message(WARNING "RAPTOR_SANITIZE is not supported with MSVC; ignoring.")
    else()
        add_library(raptor_sanitizers INTERFACE)
        target_compile_options(raptor_sanitizers INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(raptor_sanitizers INTERFACE -fsanitize=address,undefined)
    endif()
endif()

# ---------------------------------------------------------------------------
# raptor_core — domain, interfaces, infrastructure, application logic
# ---------------------------------------------------------------------------
add_library(raptor_core
    src/application/placeholder.cpp
)

target_include_directories(raptor_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(raptor_core
    PRIVATE
        raptor_warnings
        $<$<TARGET_EXISTS:raptor_sanitizers>:raptor_sanitizers>
)

# ---------------------------------------------------------------------------
# raptor — CLI executable
# ---------------------------------------------------------------------------
add_executable(raptor
    src/cli/main.cpp
)

target_link_libraries(raptor
    PRIVATE
        raptor_core
        raptor_warnings
        $<$<TARGET_EXISTS:raptor_sanitizers>:raptor_sanitizers>
)

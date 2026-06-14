# cmake/ClangTidy.cmake
# Provides a custom target:
#   tidy — runs run-clang-tidy over src/ and include/ using compile_commands.json
#
# Tests and fetched dependencies are excluded from analysis.
# Requires clang-tidy >= 18.

# ---------------------------------------------------------------------------
# Locate run-clang-tidy (wrapper script that parallelises over the compile DB)
# ---------------------------------------------------------------------------
find_program(RUN_CLANG_TIDY_EXE
    NAMES
        run-clang-tidy-22
        run-clang-tidy-21
        run-clang-tidy-20
        run-clang-tidy-19
        run-clang-tidy-18
        run-clang-tidy
    DOC "Path to run-clang-tidy script"
)

if(NOT RUN_CLANG_TIDY_EXE)
    message(STATUS "run-clang-tidy not found — tidy target unavailable")
    return()
endif()

# ---------------------------------------------------------------------------
# Locate the underlying clang-tidy binary and verify version >= 18
# ---------------------------------------------------------------------------
find_program(CLANG_TIDY_EXE
    NAMES
        clang-tidy-22
        clang-tidy-21
        clang-tidy-20
        clang-tidy-19
        clang-tidy-18
        clang-tidy
    DOC "Path to clang-tidy executable"
)

if(NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found — tidy target unavailable")
    return()
endif()

execute_process(
    COMMAND ${CLANG_TIDY_EXE} --version
    OUTPUT_VARIABLE _ct_version_str
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX MATCH "[0-9]+" _ct_major "${_ct_version_str}")
if(_ct_major LESS 18)
    message(WARNING
        "clang-tidy ${_ct_major} found at ${CLANG_TIDY_EXE}; "
        "version >= 18 required — tidy target disabled")
    return()
endif()
message(STATUS "Found clang-tidy ${_ct_major}: ${CLANG_TIDY_EXE}")
message(STATUS "Found run-clang-tidy: ${RUN_CLANG_TIDY_EXE}")

# ---------------------------------------------------------------------------
# compile_commands.json is written to CMAKE_BINARY_DIR by the preset
# (CMAKE_EXPORT_COMPILE_COMMANDS=ON).  The tidy target depends on a build so
# the DB is always fresh.
# ---------------------------------------------------------------------------
set(_RAPTOR_TIDY_SOURCES
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/include"
)

# Build a regex that matches only our first-party headers.
# run-clang-tidy's -header-filter overrides the one in .clang-tidy when
# supplied on the command line; keep them consistent.
set(_RAPTOR_HEADER_FILTER "(include/raptor/|src/)")

# ---------------------------------------------------------------------------
# tidy target
# ---------------------------------------------------------------------------
add_custom_target(tidy
    COMMAND
        ${RUN_CLANG_TIDY_EXE}
            -clang-tidy-binary ${CLANG_TIDY_EXE}
            -p "${CMAKE_BINARY_DIR}"
            -header-filter "${_RAPTOR_HEADER_FILTER}"
            ${_RAPTOR_TIDY_SOURCES}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Running clang-tidy ${_ct_major} over src/ and include/"
    VERBATIM
)

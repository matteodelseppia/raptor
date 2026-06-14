# cmake/ClangFormat.cmake
# Provides two custom targets:
#   format       — reformat all tracked C++ sources in-place
#   format-check — dry-run; exits non-zero if any file is unformatted
#
# Requires clang-format >= 18.

find_program(CLANG_FORMAT_EXE
    NAMES clang-format-18 clang-format-19 clang-format-20 clang-format
    DOC "Path to clang-format executable"
)

if(NOT CLANG_FORMAT_EXE)
    message(STATUS "clang-format not found — format / format-check targets unavailable")
    return()
endif()

# Verify version >= 18.
execute_process(
    COMMAND ${CLANG_FORMAT_EXE} --version
    OUTPUT_VARIABLE _cf_version_str
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX MATCH "[0-9]+" _cf_major "${_cf_version_str}")
if(_cf_major LESS 18)
    message(WARNING
        "clang-format ${_cf_major} found at ${CLANG_FORMAT_EXE}; "
        "version >= 18 required — format / format-check targets disabled")
    return()
endif()
message(STATUS "Found clang-format ${_cf_major}: ${CLANG_FORMAT_EXE}")

# Collect sources from src/, include/, tests/ (relative to project root).
file(GLOB_RECURSE _raptor_sources
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
    "${PROJECT_SOURCE_DIR}/tests/*.hpp"
)

# Exclude anything inside the build tree or FetchContent cache.
list(FILTER _raptor_sources EXCLUDE REGEX "${PROJECT_BINARY_DIR}")

if(NOT _raptor_sources)
    message(STATUS "ClangFormat: no sources found — skipping target creation")
    return()
endif()

# format — reformat in place.
add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXE} -i --style=file ${_raptor_sources}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Running clang-format (in-place) over src/ include/ tests/"
    VERBATIM
)

# format-check — dry-run; non-zero exit if anything needs reformatting.
add_custom_target(format-check
    COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror --style=file ${_raptor_sources}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Checking clang-format compliance (dry-run)"
    VERBATIM
)

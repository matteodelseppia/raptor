cmake_minimum_required(VERSION 3.28)
project(raptor CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
include(cmake/Warnings.cmake)

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
    PRIVATE raptor_warnings
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
)

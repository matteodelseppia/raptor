# cmake/Warnings.cmake
# Defines an INTERFACE target raptor_warnings with strict compiler warnings.

add_library(raptor_warnings INTERFACE)

if(MSVC)
    target_compile_options(raptor_warnings INTERFACE /W4)
else()
    target_compile_options(raptor_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

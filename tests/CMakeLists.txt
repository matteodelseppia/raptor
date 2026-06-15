include(GoogleTest)

add_executable(raptor_tests
    smoke_test.cpp
    error_test.cpp
    logger_test.cpp
    clock_test.cpp
    file_system_test.cpp
)

target_link_libraries(raptor_tests
    PRIVATE
        raptor_core
        raptor_warnings
        GTest::gtest_main
        GTest::gmock
        spdlog::spdlog
        $<$<TARGET_EXISTS:raptor_sanitizers>:raptor_sanitizers>
)

# Allow tests to include src/ headers (e.g. infrastructure/spdlog_logger.hpp)
# and tests/ headers (e.g. support/fake_clock.hpp)
target_include_directories(raptor_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/tests
)

gtest_discover_tests(raptor_tests)

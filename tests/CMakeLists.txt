include(GoogleTest)

add_executable(raptor_tests
    smoke_test.cpp
    error_test.cpp
)

target_link_libraries(raptor_tests
    PRIVATE
        raptor_core
        raptor_warnings
        GTest::gtest_main
        GTest::gmock
        $<$<TARGET_EXISTS:raptor_sanitizers>:raptor_sanitizers>
)

gtest_discover_tests(raptor_tests)

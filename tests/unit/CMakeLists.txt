include(FetchContent)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_build_tests OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)


add_executable(
    test_delayline
    test_delayline.cpp
)


target_link_libraries(
    test_delayline
    PRIVATE
        gtest_main
        apl
)


include(GoogleTest)

gtest_discover_tests(test_delayline)
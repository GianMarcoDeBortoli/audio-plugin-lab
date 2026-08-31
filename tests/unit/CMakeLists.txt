include(FetchContent)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_build_tests OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)


function(add_unit_test TEST_NAME)

    add_executable(${TEST_NAME}
        ${TEST_NAME}.cpp
    )

    target_link_libraries(
        ${TEST_NAME}
        PRIVATE
            GTest::gtest_main
            apl
    )

    gtest_discover_tests(${TEST_NAME})

endfunction()


add_unit_test(test_DelayLine)
add_unit_test(test_SmoothParameter)
add_unit_test(test_Matrix)
add_unit_test(test_FirstOrderShelf)
add_unit_test(test_FFT)
add_unit_test(test_TimeVaryingMatrix)
add_unit_test(test_MultichannelDelay)
add_unit_test(test_MultichannelAbsorption)
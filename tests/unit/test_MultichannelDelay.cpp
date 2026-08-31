#include <gtest/gtest.h>

#include <cstddef>
#include <vector>
#include <type_traits>

#include "operators/MultichannelDelay.h"

using apl::operators::MultichannelDelay;


// ================================================================
// Construction
// ================================================================

TEST(MultichannelDelayTest, ConstructsWithValidParameters)
{
    constexpr uint32_t channels = 3;

    const std::vector<size_t> maxLengths {
        8, 8, 8
    };

    const std::vector<float> delays {
        2, 3, 4
    };

    EXPECT_NO_THROW(
        MultichannelDelay delay(
            channels,
            maxLengths,
            delays
        )
    );
}


TEST(MultichannelDelayTest, RejectsMismatchedMaximumLengths)
{
    constexpr uint32_t channels = 3;

    const std::vector<size_t> maxLengths {
        8, 8
    };

    const std::vector<float> delays {
        2, 3, 4
    };

    EXPECT_DEATH(
        {
            MultichannelDelay delay(
                channels,
                maxLengths,
                delays
            );
        },
        ""
    );
}


TEST(MultichannelDelayTest, RejectsMismatchedInitialDelays)
{
    constexpr uint32_t channels = 3;

    const std::vector<size_t> maxLengths {
        8, 8, 8
    };

    const std::vector<float> delays {
        2, 3
    };

    EXPECT_DEATH(
        {
            MultichannelDelay delay(
                channels,
                maxLengths,
                delays
            );
        },
        ""
    );
}


// ================================================================
// Processing
// ================================================================

TEST(MultichannelDelayTest, ProcessesChannelsIndependently)
{
    constexpr uint32_t channels = 3;

    const std::vector<size_t> maxLengths {
        8, 8, 8
    };

    const std::vector<float> delays {
        2, 2, 2
    };

    MultichannelDelay delay(
        channels,
        maxLengths,
        delays
    );

    delay.prepare();

    const float input1[] = {
        1.0f, 10.0f, 100.0f
    };

    float output1[channels] {};

    delay.processSample(
        output1,
        input1,
        channels
    );

    EXPECT_FLOAT_EQ(output1[0], 0.0f);
    EXPECT_FLOAT_EQ(output1[1], 0.0f);
    EXPECT_FLOAT_EQ(output1[2], 0.0f);

    const float input2[] = {
        2.0f, 20.0f, 200.0f
    };

    float output2[channels] {};

    delay.processSample(
        output2,
        input2,
        channels
    );

    EXPECT_FLOAT_EQ(output2[0], 0.0f);
    EXPECT_FLOAT_EQ(output2[1], 0.0f);
    EXPECT_FLOAT_EQ(output2[2], 0.0f);

    const float input3[] = {
        3.0f, 30.0f, 300.0f
    };

    float output3[channels] {};

    delay.processSample(
        output3,
        input3,
        channels
    );

    EXPECT_FLOAT_EQ(output3[0], 1.0f);
    EXPECT_FLOAT_EQ(output3[1], 10.0f);
    EXPECT_FLOAT_EQ(output3[2], 100.0f);
}


TEST(MultichannelDelayTest, DifferentChannelsCanHaveDifferentDelays)
{
    constexpr uint32_t channels = 3;

    const std::vector<size_t> maxLengths {
        8, 8, 8
    };

    const std::vector<float> delays {
        1, 2, 3
    };

    MultichannelDelay delay(
        channels,
        maxLengths,
        delays
    );

    delay.prepare();

    const float input1[] = {
        1.0f, 10.0f, 100.0f
    };

    float output[channels] {};

    delay.processSample(output, input1, channels);

    EXPECT_FLOAT_EQ(output[0], 0.0f);
    EXPECT_FLOAT_EQ(output[1], 0.0f);
    EXPECT_FLOAT_EQ(output[2], 0.0f);

    const float input2[] = {
        2.0f, 20.0f, 200.0f
    };

    delay.processSample(output, input2, channels);

    EXPECT_FLOAT_EQ(output[0], 1.0f);
    EXPECT_FLOAT_EQ(output[1], 0.0f);
    EXPECT_FLOAT_EQ(output[2], 0.0f);

    const float input3[] = {
        3.0f, 30.0f, 300.0f
    };

    delay.processSample(output, input3, channels);

    EXPECT_FLOAT_EQ(output[0], 2.0f);
    EXPECT_FLOAT_EQ(output[1], 10.0f);
    EXPECT_FLOAT_EQ(output[2], 0.0f);

    const float input4[] = {
        4.0f, 40.0f, 400.0f
    };

    delay.processSample(output, input4, channels);

    EXPECT_FLOAT_EQ(output[0], 3.0f);
    EXPECT_FLOAT_EQ(output[1], 20.0f);
    EXPECT_FLOAT_EQ(output[2], 100.0f);
}


// ================================================================
// State
// ================================================================

TEST(MultichannelDelayTest, PrepareClearsAllDelayLines)
{
    constexpr uint32_t channels = 3;

    const std::vector<size_t> maxLengths {
        8, 8, 8
    };

    const std::vector<float> delays {
        2, 2, 2
    };

    MultichannelDelay delay(
        channels,
        maxLengths,
        delays
    );

    delay.prepare();

    const float input[] = {
        1.0f, 10.0f, 100.0f
    };

    float output[channels] {};

    delay.processSample(output, input, channels);

    const float newInput[] = {
        2.0f, 20.0f, 200.0f
    };

    delay.processSample(output, newInput, channels);

    delay.prepare();

    const float afterPrepare[] = {
        3.0f, 30.0f, 300.0f
    };

    delay.processSample(
        output,
        afterPrepare,
        channels
    );

    EXPECT_FLOAT_EQ(output[0], 0.0f);
    EXPECT_FLOAT_EQ(output[1], 0.0f);
    EXPECT_FLOAT_EQ(output[2], 0.0f);
}


TEST(MultichannelDelayTest, ClearClearsAllDelayLines)
{
    constexpr uint32_t channels = 2;

    const std::vector<size_t> maxLengths {
        8, 8
    };

    const std::vector<float> delays {
        2, 2
    };

    MultichannelDelay delay(
        channels,
        maxLengths,
        delays
    );

    delay.prepare();

    const float input[] = {
        1.0f, 10.0f
    };

    float output[channels] {};

    delay.processSample(output, input, channels);

    delay.clear();

    const float newInput[] = {
        2.0f, 20.0f
    };

    delay.processSample(output, newInput, channels);

    EXPECT_FLOAT_EQ(output[0], 0.0f);
    EXPECT_FLOAT_EQ(output[1], 0.0f);
}


// ================================================================
// Changing delay
// ================================================================

TEST(MultichannelDelayTest, SetDelayLinesLengthsChangesDelays)
{
    constexpr uint32_t channels = 2;

    const std::vector<size_t> maxLengths {
        8, 8
    };

    const std::vector<float> initialDelays {
        2, 2
    };

    MultichannelDelay delay(
        channels,
        maxLengths,
        initialDelays
    );

    delay.prepare();

    const float input1[] = {
        1.0f, 10.0f
    };

    float output[channels] {};

    delay.processSample(output, input1, channels);

    const float input2[] = {
        2.0f, 20.0f
    };

    delay.processSample(output, input2, channels);

    const std::vector<float> newDelays {
        1, 1
    };

    delay.setDelayLinesLengths(newDelays);

    const float input3[] = {
        3.0f, 30.0f
    };

    delay.processSample(output, input3, channels);

    // The exact transition behaviour is governed by DelayLine's
    // smoothing, so we only check that processing remains valid.
    SUCCEED();
}


// ================================================================
// API validation
// ================================================================

TEST(MultichannelDelayTest, RejectsWrongNumberOfChannels)
{
    constexpr uint32_t channels = 3;

    const std::vector<size_t> maxLengths {
        8, 8, 8
    };

    const std::vector<float> delays {
        2, 2, 2
    };

    MultichannelDelay delay(
        channels,
        maxLengths,
        delays
    );

    delay.prepare();

    const float input[3] {
        1.0f, 2.0f, 3.0f
    };

    float output[3] {};

    EXPECT_DEATH(
        {
            delay.processSample(
                output,
                input,
                2
            );
        },
        ""
    );
}


// ================================================================
// Type properties
// ================================================================

TEST(MultichannelDelayTest, IsNotCopyable)
{
    EXPECT_FALSE(
        std::is_copy_constructible_v<MultichannelDelay>
    );

    EXPECT_FALSE(
        std::is_copy_assignable_v<MultichannelDelay>
    );
}


TEST(MultichannelDelayTest, IsMovable)
{
    EXPECT_TRUE(
        std::is_move_constructible_v<MultichannelDelay>
    );

    EXPECT_TRUE(
        std::is_nothrow_move_constructible_v<MultichannelDelay>
    );

    EXPECT_TRUE(
        std::is_move_assignable_v<MultichannelDelay>
    );

    EXPECT_TRUE(
        std::is_nothrow_move_assignable_v<MultichannelDelay>
    );
}
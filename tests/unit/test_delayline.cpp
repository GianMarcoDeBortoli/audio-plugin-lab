#include <gtest/gtest.h>

#include <type_traits>
#include <utility>
#include <vector>
#include <stdexcept>

#include "operators/DelayLine.h"

using apl::operators::DelayLine;


//================================================
// Constructor

TEST(DelayLineTest, ConstructorRejectsZeroMaximumDelay)
{
    EXPECT_THROW(
        DelayLine(0, 0.0f),
        std::invalid_argument
    );
}

TEST(DelayLineTest, ConstructorRejectsNegativeInitialDelay)
{
    EXPECT_THROW(
        DelayLine(10, -1.0f),
        std::invalid_argument
    );
}

TEST(DelayLineTest, ConstructorRejectsInitialDelayGreaterThanMaximum)
{
    EXPECT_THROW(
        DelayLine(10, 11.0f),
        std::invalid_argument
    );
}

TEST(DelayLineTest, ConstructorAcceptsZeroInitialDelay)
{
    EXPECT_NO_THROW(
        DelayLine(10, 0.0f)
    );
}

TEST(DelayLineTest, ConstructorAcceptsMaximumInitialDelay)
{
    EXPECT_NO_THROW(
        DelayLine(10, 10.0f)
    );
}


//================================================
// Delay length

TEST(DelayLineTest, InitialDelayLengthIsCorrect)
{
    DelayLine delay(16, 4.0f);

    EXPECT_FLOAT_EQ(delay.getDelayLength(), 4.0f);
}

TEST(DelayLineTest, SetDelayLengthRejectsNegativeValue)
{
    DelayLine delay(16, 4.0f);

    EXPECT_THROW(
        delay.setDelayLength(-1.0f),
        std::invalid_argument
    );
}

TEST(DelayLineTest, SetDelayLengthRejectsValueGreaterThanMaximum)
{
    DelayLine delay(16, 4.0f);

    EXPECT_THROW(
        delay.setDelayLength(17.0f),
        std::invalid_argument
    );
}

TEST(DelayLineTest, SetDelayLengthAcceptsZero)
{
    DelayLine delay(16, 4.0f);

    EXPECT_NO_THROW(
        delay.setDelayLength(0.0f)
    );
}

TEST(DelayLineTest, SetDelayLengthAcceptsMaximum)
{
    DelayLine delay(16, 4.0f);

    EXPECT_NO_THROW(
        delay.setDelayLength(16.0f)
    );
}


//================================================
// processSample

TEST(DelayLineTest, ZeroDelayReturnsInput)
{
    DelayLine delay(16, 0.0f);

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f
    };

    for (const auto sample : input)
        EXPECT_FLOAT_EQ(delay.processSample(sample), sample);
}

TEST(DelayLineTest, IntegerDelayProducesExpectedOutput)
{
    DelayLine delay(16, 2.0f);

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f
    };

    const std::vector<float> expected {
        0.0f, 0.0f, 1.0f, 2.0f, 3.0f
    };

    for (size_t n = 0; n < input.size(); ++n)
        EXPECT_FLOAT_EQ(
            delay.processSample(input[n]),
            expected[n]
        );
}

TEST(DelayLineTest, OneSampleDelayProducesExpectedOutput)
{
    DelayLine delay(16, 1.0f);

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f
    };

    const std::vector<float> expected {
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f
    };

    for (size_t n = 0; n < input.size(); ++n)
        EXPECT_FLOAT_EQ(
            delay.processSample(input[n]),
            expected[n]
        );
}

TEST(DelayLineTest, DelayBufferWrapsCorrectly)
{
    DelayLine delay(4, 2.0f);

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };

    const std::vector<float> expected {
        0.0f, 0.0f, 1.0f, 2.0f,
        3.0f, 4.0f, 5.0f, 6.0f
    };

    for (size_t n = 0; n < input.size(); ++n)
        EXPECT_FLOAT_EQ(
            delay.processSample(input[n]),
            expected[n]
        );
}

TEST(DelayLineTest, MaximumDelayWorks)
{
    DelayLine delay(4, 4.0f);

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };

    const std::vector<float> expected {
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 2.0f, 3.0f, 4.0f
    };

    for (size_t n = 0; n < input.size(); ++n)
        EXPECT_FLOAT_EQ(
            delay.processSample(input[n]),
            expected[n]
        );
}


//================================================
// Fractional delay

TEST(DelayLineTest, FractionalDelayInterpolatesBetweenPastSamples)
{
    DelayLine delay(16, 1.4f);

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f
    };

    const std::vector<float> expected {
        0.0f, 0.6f, 1.6f, 2.6f, 3.6f
    };

    for (size_t n = 0; n < input.size(); ++n)
    {
        EXPECT_EQ(
            delay.processSample(input[n]),
            expected[n]
        );
    }
}


//================================================
// processBlock

TEST(DelayLineTest, ProcessBlockMatchesProcessSample)
{
    DelayLine blockDelay(16, 3.0f);
    DelayLine sampleDelay(16, 3.0f);

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };

    std::vector<float> blockOutput(input.size());
    std::vector<float> sampleOutput(input.size());

    blockDelay.processBlock(
        blockOutput.data(),
        input.data(),
        static_cast<uint32_t>(input.size())
    );

    for (size_t n = 0; n < input.size(); ++n)
        sampleOutput[n] = sampleDelay.processSample(input[n]);

    for (size_t n = 0; n < input.size(); ++n)
        EXPECT_FLOAT_EQ(blockOutput[n], sampleOutput[n]);
}


//================================================
// clear

TEST(DelayLineTest, ClearClearsDelayBuffer)
{
    DelayLine delay(16, 2.0f);

    delay.processSample(1.0f);
    delay.processSample(2.0f);

    delay.clear();

    EXPECT_FLOAT_EQ(delay.processSample(3.0f), 0.0f);
    EXPECT_FLOAT_EQ(delay.processSample(4.0f), 0.0f);
}

TEST(DelayLineTest, ClearDoesNotChangeDelayLength)
{
    DelayLine delay(16, 4.0f);

    delay.clear();

    EXPECT_FLOAT_EQ(delay.getDelayLength(), 4.0f);
}


//================================================
// prepare

TEST(DelayLineTest, PrepareClearsDelayBuffer)
{
    DelayLine delay(16, 2.0f);

    delay.processSample(1.0f);
    delay.processSample(2.0f);

    delay.prepare();

    EXPECT_FLOAT_EQ(delay.processSample(3.0f), 0.0f);
    EXPECT_FLOAT_EQ(delay.processSample(4.0f), 0.0f);
}

TEST(DelayLineTest, PrepareResetsPointer)
{
    DelayLine delay(4, 2.0f);

    delay.processSample(1.0f);
    delay.processSample(2.0f);
    delay.processSample(3.0f);

    delay.prepare();

    EXPECT_FLOAT_EQ(delay.processSample(10.0f), 0.0f);
    EXPECT_FLOAT_EQ(delay.processSample(20.0f), 0.0f);
    EXPECT_FLOAT_EQ(delay.processSample(30.0f), 10.0f);
}


//================================================
// Feedback interface

TEST(DelayLineTest, FeedbackInterfaceProducesExpectedDelay)
{
    DelayLine delay(16, 2.0f);

    std::vector<float> output;

    for (float input = 1.0f; input <= 5.0f; ++input)
    {
        output.push_back(delay.getSample());
        delay.setSample(input);
        delay.advancePointer();
    }

    const std::vector<float> expected {
        0.0f, 0.0f, 1.0f, 2.0f, 3.0f
    };

    ASSERT_EQ(output.size(), expected.size());

    for (size_t n = 0; n < output.size(); ++n)
        EXPECT_FLOAT_EQ(output[n], expected[n]);
}

TEST(DelayLineTest, GetSampleCannotBeCalledTwiceWithoutAdvancing)
{
    DelayLine delay(16, 2.0f);

    delay.getSample();

    EXPECT_THROW(
        delay.getSample(),
        std::invalid_argument
    );
}

TEST(DelayLineTest, SetSampleRequiresGetSampleFirst)
{
    DelayLine delay(16, 2.0f);

    EXPECT_THROW(
        delay.setSample(1.0f),
        std::invalid_argument
    );
}

TEST(DelayLineTest, SetSampleCannotBeCalledTwiceWithoutAdvancing)
{
    DelayLine delay(16, 2.0f);

    delay.getSample();
    delay.setSample(1.0f);

    EXPECT_THROW(
        delay.setSample(2.0f),
        std::invalid_argument
    );
}

TEST(DelayLineTest, AdvanceRequiresGetSampleFirst)
{
    DelayLine delay(16, 2.0f);

    EXPECT_THROW(
        delay.advancePointer(),
        std::invalid_argument
    );
}

TEST(DelayLineTest, AdvanceRequiresSetSampleFirst)
{
    DelayLine delay(16, 2.0f);

    delay.getSample();

    EXPECT_THROW(
        delay.advancePointer(),
        std::invalid_argument
    );
}


//================================================
// Move semantics

TEST(DelayLineTest, IsNotCopyConstructible)
{
    static_assert(!std::is_copy_constructible_v<DelayLine>);
}

TEST(DelayLineTest, IsNotCopyAssignable)
{
    static_assert(!std::is_copy_assignable_v<DelayLine>);
}

TEST(DelayLineTest, IsNothrowMoveConstructible)
{
    static_assert(std::is_nothrow_move_constructible_v<DelayLine>);
}

TEST(DelayLineTest, IsMoveConstructible)
{
    static_assert(std::is_move_constructible_v<DelayLine>);
}

TEST(DelayLineTest, IsNothrowMoveAssignable)
{
    static_assert(std::is_nothrow_move_assignable_v<DelayLine>);
}

TEST(DelayLineTest, MoveConstructionWorks)
{
    DelayLine original(16, 2.0f);

    original.processSample(1.0f);
    original.processSample(2.0f);

    DelayLine moved(std::move(original));

    EXPECT_FLOAT_EQ(moved.processSample(3.0f), 1.0f);
}

TEST(DelayLineTest, MoveAssignmentWorks)
{
    DelayLine original(16, 2.0f);

    original.processSample(1.0f);
    original.processSample(2.0f);

    DelayLine destination(16, 4.0f);

    destination = std::move(original);

    EXPECT_FLOAT_EQ(destination.processSample(3.0f), 1.0f);
}
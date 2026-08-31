#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "operators/FirstOrderShelf.h"

namespace apl::operators
{

TEST(FirstOrderShelfTest, ConstructorAcceptsValidParameters)
{
    EXPECT_NO_THROW(
        FirstOrderShelf shelf(0.5f, 0.8f, 0.5f)
    );
}

TEST(FirstOrderShelfTest, ConstructorRejectsInvalidDCMagnitude)
{
    EXPECT_THROW(
        FirstOrderShelf(0.0f, 0.8f, 0.5f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(1.0f, 0.8f, 0.5f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(-0.1f, 0.8f, 0.5f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(1.1f, 0.8f, 0.5f),
        std::invalid_argument
    );
}

TEST(FirstOrderShelfTest, ConstructorRejectsInvalidNYMagnitude)
{
    EXPECT_THROW(
        FirstOrderShelf(0.5f, 0.0f, 0.5f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(0.5f, 1.0f, 0.5f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(0.5f, -0.1f, 0.5f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(0.5f, 1.1f, 0.5f),
        std::invalid_argument
    );
}

TEST(FirstOrderShelfTest, ConstructorRejectsInvalidCrossoverFrequency)
{
    constexpr float pi = apl::math::constants::pi;

    EXPECT_THROW(
        FirstOrderShelf(0.5f, 0.8f, 0.0f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(0.5f, 0.8f, -0.1f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(0.5f, 0.8f, pi / 2.0f),
        std::invalid_argument
    );

    EXPECT_THROW(
        FirstOrderShelf(0.5f, 0.8f, pi),
        std::invalid_argument
    );
}

//================================================
// DC / Nyquist response

TEST(FirstOrderShelfTest, ConstantInputApproachesDCMagnitude)
{
    constexpr float HDC = 0.5f;
    constexpr float HNY = 0.8f;
    constexpr float crossAngularFreq = 0.5f;

    FirstOrderShelf shelf(HDC, HNY, crossAngularFreq);

    constexpr int numSamples = 10000;

    float output = 0.0f;

    for (int n = 0; n < numSamples; ++n)
        output = shelf.processSample(1.0f);

    EXPECT_NEAR(output, HDC, 1e-4f);
}

TEST(FirstOrderShelfTest, AlternatingInputApproachesNyquistMagnitude)
{
    constexpr float HDC = 0.5f;
    constexpr float HNY = 0.8f;
    constexpr float crossAngularFreq = 0.5f;

    FirstOrderShelf shelf(HDC, HNY, crossAngularFreq);

    constexpr int numSamples = 10000;

    float output = 0.0f;

    for (int n = 0; n < numSamples; ++n)
    {
        const float input = (n % 2 == 0) ? 1.0f : -1.0f;
        output = shelf.processSample(input);
    }

    const float expected = -HNY;

    EXPECT_NEAR(output, expected, 1e-4f);
}

//================================================
// Sample / block equivalence

TEST(FirstOrderShelfTest, ProcessBlockMatchesProcessSample)
{
    constexpr float HDC = 0.5f;
    constexpr float HNY = 0.8f;
    constexpr float crossAngularFreq = 0.5f;

    FirstOrderShelf sampleShelf(
        HDC,
        HNY,
        crossAngularFreq
    );

    FirstOrderShelf blockShelf(
        HDC,
        HNY,
        crossAngularFreq
    );

    const std::vector<float> input {
        1.0f,
        -0.5f,
        0.25f,
        2.0f,
        -1.0f,
        0.75f,
        -0.25f,
        0.0f
    };

    std::vector<float> sampleOutput(input.size());
    std::vector<float> blockOutput(input.size());

    for (size_t n = 0; n < input.size(); ++n)
    {
        sampleOutput[n] = sampleShelf.processSample(input[n]);
    }

    blockShelf.processBlock(
        blockOutput.data(),
        input.data(),
        static_cast<uint32_t>(input.size())
    );

    for (size_t n = 0; n < input.size(); ++n)
    {
        EXPECT_FLOAT_EQ(
            sampleOutput[n],
            blockOutput[n]
        );
    }
}

//================================================
// State

TEST(FirstOrderShelfTest, ClearResetsFilterState)
{
    FirstOrderShelf shelf(
        0.5f,
        0.8f,
        0.5f
    );

    // Put the filter into a non-zero state.
    for (int n = 0; n < 100; ++n)
        shelf.processSample(1.0f);

    shelf.clear();

    FirstOrderShelf reference(
        0.5f,
        0.8f,
        0.5f
    );

    const float input = 0.37f;

    const float output = shelf.processSample(input);
    const float expected = reference.processSample(input);

    EXPECT_FLOAT_EQ(output, expected);
}

//================================================
// Set methods

TEST(FirstOrderShelfTest, SetDCMagnitudeChangesDCResponse)
{
    constexpr float HNY = 0.8f;
    constexpr float crossAngularFreq = 0.5f;

    FirstOrderShelf shelf(
        0.3f,
        HNY,
        crossAngularFreq
    );

    shelf.setDCMagnitudeValue(0.6f);

    float output = 0.0f;

    for (int n = 0; n < 10000; ++n)
        output = shelf.processSample(1.0f);

    EXPECT_NEAR(output, 0.6f, 1e-4f);
}

TEST(FirstOrderShelfTest, SetNYMagnitudeChangesNyquistResponse)
{
    constexpr float HDC = 0.5f;
    constexpr float crossAngularFreq = 0.5f;

    FirstOrderShelf shelf(
        HDC,
        0.4f,
        crossAngularFreq
    );

    shelf.setNYMagnitudeValue(0.7f);

    float output = 0.0f;

    for (int n = 0; n < 10000; ++n)
    {
        const float input = (n % 2 == 0) ? 1.0f : -1.0f;
        output = shelf.processSample(input);
    }

    EXPECT_NEAR(output, -0.7f, 1e-4f);
}

TEST(FirstOrderShelfTest, SetMethodsRejectInvalidValues)
{
    FirstOrderShelf shelf(
        0.5f,
        0.8f,
        0.5f
    );

    EXPECT_THROW(
        shelf.setDCMagnitudeValue(0.0f),
        std::invalid_argument
    );

    EXPECT_THROW(
        shelf.setDCMagnitudeValue(1.0f),
        std::invalid_argument
    );

    EXPECT_THROW(
        shelf.setNYMagnitudeValue(0.0f),
        std::invalid_argument
    );

    EXPECT_THROW(
        shelf.setNYMagnitudeValue(1.0f),
        std::invalid_argument
    );

    constexpr float pi = apl::math::constants::pi;

    EXPECT_THROW(
        shelf.setCrossoverFrequency(0.0f),
        std::invalid_argument
    );

    EXPECT_THROW(
        shelf.setCrossoverFrequency(pi / 2.0f),
        std::invalid_argument
    );
}

//================================================
// Copy / move

TEST(FirstOrderShelfTest, CopyConstructionPreservesState)
{
    FirstOrderShelf original(
        0.5f,
        0.8f,
        0.5f
    );

    for (int n = 0; n < 100; ++n)
        original.processSample(0.25f);

    FirstOrderShelf copy(original);

    const float input = 0.75f;

    const float originalOutput = original.processSample(input);
    const float copyOutput = copy.processSample(input);

    EXPECT_FLOAT_EQ(
        originalOutput,
        copyOutput
    );
}

TEST(FirstOrderShelfTest, CopyAssignmentPreservesState)
{
    FirstOrderShelf original(
        0.5f,
        0.8f,
        0.5f
    );

    for (int n = 0; n < 100; ++n)
        original.processSample(0.25f);

    FirstOrderShelf copy(
        0.3f,
        0.6f,
        0.4f
    );

    copy = original;

    const float input = 0.75f;

    const float originalOutput = original.processSample(input);
    const float copyOutput = copy.processSample(input);

    EXPECT_FLOAT_EQ(
        originalOutput,
        copyOutput
    );
}

}
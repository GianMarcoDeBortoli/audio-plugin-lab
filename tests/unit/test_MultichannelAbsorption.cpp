#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <vector>

#include "operators/MultichannelAbsorption.h"

using apl::operators::MultichannelAbsorption;


//================================================
// CONSTRUCTOR

TEST(MultichannelAbsorptionTest, ConstructsWithValidArguments)
{
    constexpr uint32_t channels = 4;

    const std::vector<float> hDC {
        0.8f, 0.7f, 0.6f, 0.5f
    };

    const std::vector<float> hNY {
        0.4f, 0.5f, 0.6f, 0.7f
    };

    constexpr float crossFreq = 0.2f;

    EXPECT_NO_THROW(
        MultichannelAbsorption absorption(
            channels,
            hDC,
            hNY,
            crossFreq
        )
    );
}


//================================================
// PROCESSING

TEST(MultichannelAbsorptionTest, ProcessesEachChannelIndependently)
{
    constexpr uint32_t channels = 4;

    const std::vector<float> hDC(channels, 0.8f);
    const std::vector<float> hNY(channels, 0.8f);

    MultichannelAbsorption absorption(
        channels,
        hDC,
        hNY,
        0.2f
    );

    absorption.prepare();

    const std::vector<float> input {
        1.0f, -2.0f, 0.5f, 3.0f
    };

    std::vector<float> output(channels, 0.0f);

    absorption.processSample(
        output.data(),
        input.data(),
        channels
    );

    // With equal DC/NY magnitudes, the shelf has constant gain.
    // The first sample therefore has the same gain on every channel.
    for (size_t ch = 0; ch < channels; ++ch)
    {
        EXPECT_NEAR(
            output[ch],
            hDC[ch] * input[ch],
            1e-5f
        );
    }
}


//================================================
// CLEAR

TEST(MultichannelAbsorptionTest, ClearResetsAllFilterStates)
{
    constexpr uint32_t channels = 3;

    const std::vector<float> hDC(channels, 0.7f);
    const std::vector<float> hNY(channels, 0.4f);

    MultichannelAbsorption absorption(
        channels,
        hDC,
        hNY,
        0.2f
    );

    absorption.prepare();

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f
    };

    std::vector<float> output(channels, 0.0f);

    // Advance the internal states.
    absorption.processSample(
        output.data(),
        input.data(),
        channels
    );

    absorption.clear();

    std::vector<float> zeroInput(channels, 0.0f);
    std::vector<float> zeroOutput(channels, 0.0f);

    absorption.processSample(
        zeroOutput.data(),
        zeroInput.data(),
        channels
    );

    // After clearing, zero input should produce zero output.
    for (float sample : zeroOutput)
        EXPECT_FLOAT_EQ(sample, 0.0f);
}


//================================================
// SET DC MAGNITUDE

TEST(MultichannelAbsorptionTest, SetDCMagnitudeValueAffectsAllChannels)
{
    constexpr uint32_t channels = 3;

    MultichannelAbsorption absorption(
        channels,
        {0.2f, 0.4f, 0.6f},
        {0.8f, 0.8f, 0.8f},
        0.5f
    );

    absorption.setDCMagnitudeValue({0.3f, 0.5f, 0.7f});

    std::vector<float> input(channels, 1.0f);
    std::vector<float> output(channels, 0.0f);

    // Allow coefficient smoothing to settle.
    for (int n = 0; n < 2000; ++n)
        absorption.processSample(output.data(), input.data(), channels);

    EXPECT_NEAR(output[0], 0.3f, 1e-3f);
    EXPECT_NEAR(output[1], 0.5f, 1e-3f);
    EXPECT_NEAR(output[2], 0.7f, 1e-3f);
}


//================================================
// SET NY MAGNITUDE

TEST(MultichannelAbsorptionTest, SetNYMagnitudeValueAffectsAllChannels)
{
    constexpr uint32_t channels = 3;

    const std::vector<float> hDC(channels, 0.8f);
    const std::vector<float> initialHNY(channels, 0.8f);

    MultichannelAbsorption absorption(
        channels,
        hDC,
        initialHNY,
        0.2f
    );

    absorption.prepare();

    const std::vector<float> newHNY {
        0.2f, 0.4f, 0.6f
    };

    absorption.setNYMagnitudeValue(newHNY);

    absorption.clear();

    // A constant input only tests DC, so use an alternating signal
    // to exercise the Nyquist gain.
    std::vector<float> input(channels, 1.0f);
    std::vector<float> output(channels, 0.0f);

    for (int n = 0; n < 100; ++n)
    {
        for (size_t ch = 0; ch < channels; ++ch)
            input[ch] = (n % 2 == 0) ? 1.0f : -1.0f;

        absorption.processSample(
            output.data(),
            input.data(),
            channels
        );
    }

    for (size_t ch = 0; ch < channels; ++ch)
    {
        EXPECT_NEAR(
            std::abs(output[ch]),
            newHNY[ch],
            1e-3f
        );
    }
}


//================================================
// SET CROSSOVER FREQUENCY

TEST(MultichannelAbsorptionTest, SetCrossFrequencyDoesNotThrow)
{
    constexpr uint32_t channels = 3;

    const std::vector<float> hDC(channels, 0.8f);
    const std::vector<float> hNY(channels, 0.4f);

    MultichannelAbsorption absorption(
        channels,
        hDC,
        hNY,
        0.2f
    );

    EXPECT_NO_THROW(
        absorption.setCrossFrequency(0.5f)
    );
}


//================================================
// PREPARE

TEST(MultichannelAbsorptionTest, PrepareResetsAllFilterStates)
{
    constexpr uint32_t channels = 3;

    const std::vector<float> hDC(channels, 0.7f);
    const std::vector<float> hNY(channels, 0.4f);

    MultichannelAbsorption absorption(
        channels,
        hDC,
        hNY,
        0.2f
    );

    absorption.prepare();

    const std::vector<float> input {
        1.0f, 2.0f, 3.0f
    };

    std::vector<float> output(channels, 0.0f);

    absorption.processSample(
        output.data(),
        input.data(),
        channels
    );

    absorption.prepare();

    const std::vector<float> zeroInput(channels, 0.0f);
    std::vector<float> zeroOutput(channels, 0.0f);

    absorption.processSample(
        zeroOutput.data(),
        zeroInput.data(),
        channels
    );

    for (float sample : zeroOutput)
        EXPECT_FLOAT_EQ(sample, 0.0f);
}
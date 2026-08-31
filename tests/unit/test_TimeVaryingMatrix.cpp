#include <gtest/gtest.h>

#include <cmath>
#include <random>
#include <vector>

#include "operators/TimeVaryingMatrix.h"

using apl::operators::TimeVaryingMatrix;


// ============================================================
// Construction
// ============================================================

TEST(TimeVaryingMatrixTest, RejectsZeroDimension)
{
    std::mt19937 rng(1234);

    EXPECT_THROW(
        TimeVaryingMatrix(0, 0.01f, 0.5f, 0.0f, rng),
        std::invalid_argument
    );
}


TEST(TimeVaryingMatrixTest, RejectsNonPowerOfTwoDimension)
{
    std::mt19937 rng(1234);

    EXPECT_THROW(
        TimeVaryingMatrix(10, 0.01f, 0.5f, 0.0f, rng),
        std::invalid_argument
    );
}


TEST(TimeVaryingMatrixTest, AcceptsPowerOfTwoDimension)
{
    std::mt19937 rng(1234);

    EXPECT_NO_THROW(
        TimeVaryingMatrix(64, 0.01f, 0.5f, 0.0f, rng)
    );
}


TEST(TimeVaryingMatrixTest, RejectsInvalidDepth)
{
    std::mt19937 rng(1234);

    EXPECT_THROW(
        TimeVaryingMatrix(64, 0.01f, -0.1f, 0.0f, rng),
        std::invalid_argument
    );

    EXPECT_THROW(
        TimeVaryingMatrix(64, 0.01f, 1.1f, 0.0f, rng),
        std::invalid_argument
    );
}


TEST(TimeVaryingMatrixTest, RejectsInvalidSpread)
{
    std::mt19937 rng(1234);

    EXPECT_THROW(
        TimeVaryingMatrix(64, 0.01f, 0.5f, -0.1f, rng),
        std::invalid_argument
    );

    EXPECT_THROW(
        TimeVaryingMatrix(64, 0.01f, 0.5f, 1.1f, rng),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, ZeroDepthIsIdentity)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234);

    TimeVaryingMatrix matrix(
        dimension,
        0.01f,
        0.0f,  // no modulation
        0.0f,
        rng
    );

    std::vector<float> input(dimension);
    std::vector<float> output(dimension);

    for (std::size_t i = 0; i < dimension; ++i)
        input[i] = std::sin(0.13f * static_cast<float>(i))
                 + 0.3f * std::cos(0.37f * static_cast<float>(i));

    matrix.processSample(output.data(), input.data());

    for (std::size_t i = 0; i < dimension; ++i)
    {
        EXPECT_NEAR(
            output[i],
            input[i],
            1e-4f
        );
    }
}

TEST(TimeVaryingMatrixTest, ZeroDepthPreservesRandomInput)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    TimeVaryingMatrix matrix(
        dimension,
        0.01f,
        0.0f,
        0.0f,
        rng
    );

    std::vector<float> input(dimension);
    std::vector<float> output(dimension);

    for (float& x : input)
        x = dist(rng);

    matrix.processSample(output.data(), input.data());

    for (std::size_t i = 0; i < dimension; ++i)
        EXPECT_NEAR(output[i], input[i], 1e-4f);
}

TEST(TimeVaryingMatrixTest, PreservesEnergy)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        1.0f,
        0.5f,
        rng
    );

    std::mt19937 inputRng(5678);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    std::vector<float> input(dimension);
    std::vector<float> output(dimension);

    for (float& x : input)
        x = dist(inputRng);

    matrix.processSample(output.data(), input.data());

    float inputEnergy = 0.0f;
    float outputEnergy = 0.0f;

    for (std::size_t i = 0; i < dimension; ++i)
    {
        inputEnergy += input[i] * input[i];
        outputEnergy += output[i] * output[i];
    }

    EXPECT_NEAR(
        outputEnergy,
        inputEnergy,
        1e-3f
    );
}

TEST(TimeVaryingMatrixTest, PreservesEnergyOverMultipleSamples)
{
    constexpr std::size_t dimension = 64;
    constexpr std::size_t numSamples = 100;

    std::mt19937 rng(1234);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        1.0f,
        0.5f,
        rng
    );

    std::mt19937 inputRng(5678);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    std::vector<float> input(dimension);
    std::vector<float> output(dimension);

    for (std::size_t n = 0; n < numSamples; ++n)
    {
        for (float& x : input)
            x = dist(inputRng);

        matrix.processSample(output.data(), input.data());

        float inputEnergy = 0.0f;
        float outputEnergy = 0.0f;

        for (std::size_t i = 0; i < dimension; ++i)
        {
            inputEnergy += input[i] * input[i];
            outputEnergy += output[i] * output[i];
        }

        EXPECT_NEAR(
            outputEnergy,
            inputEnergy,
            1e-3f
        );
    }
}
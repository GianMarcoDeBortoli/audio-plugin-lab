#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

#include "operators/TimeVaryingMatrix.h"

using apl::operators::TimeVaryingMatrix;


// ================================================================
// Constructor
// ================================================================

TEST(TimeVaryingMatrixTest, ConstructsWithValidParameters)
{
    std::mt19937 rng(1234u);

    EXPECT_NO_THROW(
        TimeVaryingMatrix(
            64,
            0.001f,
            0.5f,
            0.5f,
            rng
        )
    );
}

TEST(TimeVaryingMatrixTest, RejectsZeroDimension)
{
    std::mt19937 rng(1234u);

    EXPECT_THROW(
        TimeVaryingMatrix(
            0,
            0.001f,
            0.5f,
            0.5f,
            rng
        ),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, RejectsNonPowerOfTwoDimension)
{
    std::mt19937 rng(1234u);

    EXPECT_THROW(
        TimeVaryingMatrix(
            63,
            0.001f,
            0.5f,
            0.5f,
            rng
        ),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, RejectsFrequencyBelowZero)
{
    std::mt19937 rng(1234u);

    EXPECT_THROW(
        TimeVaryingMatrix(
            64,
            -0.001f,
            0.5f,
            0.5f,
            rng
        ),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, RejectsFrequencyAtNyquist)
{
    std::mt19937 rng(1234u);

    EXPECT_THROW(
        TimeVaryingMatrix(
            64,
            0.5f,
            0.5f,
            0.5f,
            rng
        ),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, RejectsDepthOutsideRange)
{
    std::mt19937 rng(1234u);

    EXPECT_THROW(
        TimeVaryingMatrix(
            64,
            0.001f,
            1.1f,
            0.5f,
            rng
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        TimeVaryingMatrix(
            64,
            0.001f,
            -0.1f,
            0.5f,
            rng
        ),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, RejectsSpreadOutsideRange)
{
    std::mt19937 rng(1234u);

    EXPECT_THROW(
        TimeVaryingMatrix(
            64,
            0.001f,
            0.5f,
            1.1f,
            rng
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        TimeVaryingMatrix(
            64,
            0.001f,
            0.5f,
            -0.1f,
            rng
        ),
        std::invalid_argument
    );
}


// ================================================================
// Processing
// ================================================================

TEST(TimeVaryingMatrixTest, ZeroInputProducesZeroOutput)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng
    );

    std::vector<float> input(dimension, 0.0f);
    std::vector<float> output(dimension, 123.0f);

    matrix.processSample(output.data(), input.data());

    for (float sample : output)
        EXPECT_FLOAT_EQ(sample, 0.0f);
}

TEST(TimeVaryingMatrixTest, ProcessingPreservesEnergyWhenNoModulation)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.0f,
        0.0f,
        rng
    );

    std::vector<float> input(dimension);

    for (std::size_t i = 0; i < dimension; ++i)
        input[i] = static_cast<float>(i + 1);

    std::vector<float> output(dimension, 0.0f);

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

TEST(TimeVaryingMatrixTest, ProcessingProducesFiniteOutput)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        1.0f,
        1.0f,
        rng
    );

    std::vector<float> input(dimension);
    std::vector<float> output(dimension);

    for (std::size_t i = 0; i < dimension; ++i)
        input[i] = std::sin(static_cast<float>(i));

    for (int n = 0; n < 100; ++n)
    {
        matrix.processSample(output.data(), input.data());

        for (float sample : output)
            EXPECT_TRUE(std::isfinite(sample));
    }
}


// ================================================================
// Setters
// ================================================================

TEST(TimeVaryingMatrixTest, SetCyclesPerSampleAcceptsValidValues)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng
    );

    EXPECT_NO_THROW(matrix.setCyclesPerSample(0.0f));
    EXPECT_NO_THROW(matrix.setCyclesPerSample(0.1f));
    EXPECT_NO_THROW(matrix.setCyclesPerSample(0.499f));
}

TEST(TimeVaryingMatrixTest, SetCyclesPerSampleRejectsInvalidValues)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng
    );

    EXPECT_THROW(
        matrix.setCyclesPerSample(-0.001f),
        std::invalid_argument
    );

    EXPECT_THROW(
        matrix.setCyclesPerSample(0.5f),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, SetDepthAcceptsValidValues)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng
    );

    EXPECT_NO_THROW(matrix.setDepth(0.0f));
    EXPECT_NO_THROW(matrix.setDepth(0.5f));
    EXPECT_NO_THROW(matrix.setDepth(1.0f));
}

TEST(TimeVaryingMatrixTest, SetDepthRejectsInvalidValues)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng
    );

    EXPECT_THROW(
        matrix.setDepth(-0.1f),
        std::invalid_argument
    );

    EXPECT_THROW(
        matrix.setDepth(1.1f),
        std::invalid_argument
    );
}

TEST(TimeVaryingMatrixTest, SetSpreadAcceptsValidValues)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng
    );

    EXPECT_NO_THROW(matrix.setSpread(0.0f));
    EXPECT_NO_THROW(matrix.setSpread(0.5f));
    EXPECT_NO_THROW(matrix.setSpread(1.0f));
}

TEST(TimeVaryingMatrixTest, SetSpreadRejectsInvalidValues)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng(1234u);

    TimeVaryingMatrix matrix(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng
    );

    EXPECT_THROW(
        matrix.setSpread(-0.1f),
        std::invalid_argument
    );

    EXPECT_THROW(
        matrix.setSpread(1.1f),
        std::invalid_argument
    );
}


// ================================================================
// State
// ================================================================

TEST(TimeVaryingMatrixTest, ResetAllowsProcessingAgain)
{
    constexpr std::size_t dimension = 64;

    std::mt19937 rng1(1234u);
    std::mt19937 rng2(1234u);

    TimeVaryingMatrix matrix1(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng1
    );

    TimeVaryingMatrix matrix2(
        dimension,
        0.001f,
        0.5f,
        0.5f,
        rng2
    );

    std::vector<float> input(dimension);
    std::vector<float> output1(dimension);
    std::vector<float> output2(dimension);

    for (std::size_t i = 0; i < dimension; ++i)
        input[i] = static_cast<float>(i + 1);

    matrix1.processSample(output1.data(), input.data());

    matrix1.reset();

    matrix1.processSample(output2.data(), input.data());

    matrix2.processSample(output1.data(), input.data());

    for (std::size_t i = 0; i < dimension; ++i)
    {
        EXPECT_NEAR(
            output2[i],
            output1[i],
            1e-5f
        );
    }
}
#include <gtest/gtest.h>

#include <vector>
#include <random>

#include "operators/Matrix.h"

using apl::operators::Matrix;

//================================================
// CONSTRUCTOR TESTS

TEST(MatrixTest, ConstructsSquareMatrix)
{
    EXPECT_NO_THROW({
        Matrix matrix(4);
    });
}

TEST(MatrixTest, ConstructsRectangularMatrix)
{
    EXPECT_NO_THROW({
        Matrix matrix(3, 5);
    });

    EXPECT_NO_THROW({
        Matrix matrix(5, 3);
    });
}

TEST(MatrixTest, RejectsZeroDimension)
{
    EXPECT_THROW(Matrix(0), std::invalid_argument);
    EXPECT_THROW(Matrix(0, 4), std::invalid_argument);
    EXPECT_THROW(Matrix(4, 0), std::invalid_argument);
}

TEST(MatrixTest, RejectsNegativeDimension)
{
    EXPECT_THROW(Matrix(-1), std::invalid_argument);
    EXPECT_THROW(Matrix(-1, 4), std::invalid_argument);
    EXPECT_THROW(Matrix(4, -1), std::invalid_argument);
}


//================================================
// PROCESS TESTS

TEST(MatrixTest, SquareMatrixPreservesEnergy)
{
    constexpr int dim = 8;

    Matrix matrix(dim);

    const std::vector<float> input {
        1.0f, -2.0f, 0.5f, 3.0f,
        -1.5f, 0.25f, 2.0f, -0.75f
    };

    std::vector<float> output(dim, 0.0f);

    matrix.processSample(
        output.data(),
        input.data(),
        dim,
        dim
    );

    float inputEnergy = 0.0f;
    float outputEnergy = 0.0f;

    for (int n = 0; n < dim; ++n)
    {
        inputEnergy += input[n] * input[n];
        outputEnergy += output[n] * output[n];
    }

    EXPECT_NEAR(outputEnergy, inputEnergy, 1e-5f);
}


TEST(MatrixTest, MoreOutputsThanInputsPreservesEnergy)
{
    constexpr int numInputs = 4;
    constexpr int numOutputs = 8;

    Matrix matrix(numOutputs, numInputs);

    const std::vector<float> input {
        1.0f, -2.0f, 0.5f, 3.0f
    };

    std::vector<float> output(numOutputs, 0.0f);

    matrix.processSample(
        output.data(),
        input.data(),
        numOutputs,
        numInputs
    );

    float inputEnergy = 0.0f;
    float outputEnergy = 0.0f;

    for (int n = 0; n < numInputs; ++n)
        inputEnergy += input[n] * input[n];

    for (int n = 0; n < numOutputs; ++n)
        outputEnergy += output[n] * output[n];

    EXPECT_NEAR(outputEnergy, inputEnergy, 1e-5f);
}


TEST(MatrixTest, FewerOutputsThanInputsPreservesExpectedPowerForRandomMatrix)
{
    constexpr int numInputs = 8;
    constexpr int numOutputs = 4;
    constexpr int numRealizations = 1000;

    const std::vector<float> input {
        1.0f, -2.0f, 0.5f, 3.0f,
        -1.5f, 0.25f, 2.0f, -0.75f
    };

    float inputEnergy = 0.0f;

    for (float sample : input)
        inputEnergy += sample * sample;

    float averageOutputEnergy = 0.0f;

    for (int realization = 0; realization < numRealizations; ++realization)
    {
        Matrix matrix(numOutputs, numInputs);

        std::vector<float> output(numOutputs, 0.0f);

        matrix.processSample(
            output.data(),
            input.data(),
            numOutputs,
            numInputs
        );

        float outputEnergy = 0.0f;

        for (float sample : output)
            outputEnergy += sample * sample;

        averageOutputEnergy += outputEnergy;
    }

    averageOutputEnergy /= static_cast<float>(numRealizations);

    EXPECT_NEAR(
        averageOutputEnergy,
        inputEnergy,
        0.1f
    );
}


TEST(MatrixTest, FewerOutputsThanInputsPreservesExpectedPowerForRandomInputs)
{
    constexpr int numInputs = 8;
    constexpr int numOutputs = 4;
    constexpr int numRealizations = 10000;

    Matrix matrix(numOutputs, numInputs);

    std::mt19937 generator(12345);
    std::normal_distribution<float> distribution(0.0f, 1.0f);

    float averageInputEnergy = 0.0f;
    float averageOutputEnergy = 0.0f;

    std::vector<float> input(numInputs);
    std::vector<float> output(numOutputs);

    for (int realization = 0; realization < numRealizations; ++realization)
    {
        for (float& sample : input)
            sample = distribution(generator);

        matrix.processSample(
            output.data(),
            input.data(),
            numOutputs,
            numInputs
        );

        for (float sample : input)
            averageInputEnergy += sample * sample;

        for (float sample : output)
            averageOutputEnergy += sample * sample;
    }

    averageInputEnergy /= static_cast<float>(numRealizations);
    averageOutputEnergy /= static_cast<float>(numRealizations);

    EXPECT_NEAR(
        averageOutputEnergy,
        averageInputEnergy,
        0.1f
    );
}


//================================================
// PREPARE TESTS

TEST(MatrixTest, PrepareSquareMatrixChangesDimension)
{
    Matrix matrix(4);

    EXPECT_NO_THROW({
        matrix.prepare(8);
    });

    const std::vector<float> input(8, 1.0f);
    std::vector<float> output(8, 0.0f);

    EXPECT_NO_THROW({
        matrix.processSample(
            output.data(),
            input.data(),
            8,
            8
        );
    });
}


TEST(MatrixTest, PrepareRectangularMatrixChangesDimensions)
{
    Matrix matrix(4, 8);

    EXPECT_NO_THROW({
        matrix.prepare(8, 4);
    });

    const std::vector<float> input(4, 1.0f);
    std::vector<float> output(8, 0.0f);

    EXPECT_NO_THROW({
        matrix.processSample(
            output.data(),
            input.data(),
            8,
            4
        );
    });
}


TEST(MatrixTest, PrepareWithSameSquareDimensionDoesNothing)
{
    Matrix matrix(8);

    const std::vector<float> input(8, 1.0f);
    std::vector<float> output(8, 0.0f);

    EXPECT_NO_THROW({
        matrix.prepare(8);
        matrix.processSample(
            output.data(),
            input.data(),
            8,
            8
        );
    });
}


TEST(MatrixTest, PrepareWithSameRectangularDimensionsDoesNothing)
{
    Matrix matrix(4, 8);

    const std::vector<float> input(8, 1.0f);
    std::vector<float> output(4, 0.0f);

    EXPECT_NO_THROW({
        matrix.prepare(4, 8);
        matrix.processSample(
            output.data(),
            input.data(),
            4,
            8
        );
    });
}


//================================================
// COPY / MOVE TESTS

TEST(MatrixTest, IsCopyConstructible)
{
    Matrix matrix(4);

    EXPECT_NO_THROW({
        Matrix copy(matrix);
    });
}


TEST(MatrixTest, IsCopyAssignable)
{
    Matrix matrix1(4);
    Matrix matrix2(4);

    EXPECT_NO_THROW({
        matrix2 = matrix1;
    });
}


TEST(MatrixTest, IsMoveConstructible)
{
    Matrix matrix(4);

    EXPECT_NO_THROW({
        Matrix moved(std::move(matrix));
    });
}


TEST(MatrixTest, IsMoveAssignable)
{
    Matrix matrix1(4);
    Matrix matrix2(4);

    EXPECT_NO_THROW({
        matrix2 = std::move(matrix1);
    });
}
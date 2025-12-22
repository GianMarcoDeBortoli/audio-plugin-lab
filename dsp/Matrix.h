#pragma once

#include <cstdint>

#include <JuceHeader.h>

#include <Eigen/Dense>

namespace DSP
{

class Matrix
{
public:
    Matrix(
        int initDim
    );
    Matrix(
        int initDim1,
        int initDim2
    );
    ~Matrix();

    // No default ctor
    Matrix() = delete;

    // No copy semantics
    Matrix(const Matrix&) = delete;
    const Matrix& operator=(const Matrix&) = delete;

    // No move semantics
    Matrix(Matrix&&) noexcept = default;
    Matrix& operator=(Matrix&&) noexcept = default;

    // =============================================

    // Generates the matrix
    Eigen::MatrixXf genRandomOrthogonal(int dim);

    // Generates the matrix with specified dimensions
    Eigen::MatrixXf genRandomCoupling(int dim1, int dim2);

    // Set the number of delay lines
    void setDimensions(int newDim);
    void setDimensions(int newDim1, int newDim2);

    // =============================================

    // Reallocate delay buffer for the maxLength and clear its contents
    void prepare(int newDim);
    void prepare(int newDim1, int newDim2);

    // Clear the contents of the delay buffer
    void clear();

    // Process multi-channel sample
    void processSample(float* outSamples, const float* inSamples, uint32_t numOutputChannels, uint32_t numInputChannels);

private:
    int dim1;
    int dim2;
    Eigen::MatrixXf matrix;

    // static_assert(std::is_copy_constructible_v<Matrix>);
    // static_assert(std::is_move_constructible_v<Matrix>);
    static_assert(std::is_nothrow_move_assignable_v<Matrix>);
};

}
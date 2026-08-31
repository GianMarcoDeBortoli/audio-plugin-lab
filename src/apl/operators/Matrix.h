#pragma once

#include <cstdint>

#include <Eigen/Dense>

namespace apl::operators
{

class Matrix
{

public:

    // Constructor
    Matrix() = delete;
    Matrix(
        int initDim
    );
    Matrix(
        int initDim1,
        int initDim2
    );

    // Destructor
    ~Matrix() = default;

    // Copy
    Matrix(const Matrix&) = default;
    Matrix& operator=(const Matrix&) = default;

    // Move
    Matrix(Matrix&&) noexcept = default;
    Matrix& operator=(Matrix&&) noexcept = default;

    // =============================================
    // STATE METHODS
    // Prepare matrices with correct dimensions
    void prepare(int newDim);
    void prepare(int newDim1, int newDim2);

    // =============================================
    // PROCESS METHODS
    // Process audio sample - multichannel
    void processSample(
        float* outSamples,
        const float* inSamples,
        uint32_t numOutputChannels,
        uint32_t numInputChannels
    ) noexcept;

    //==============================================

private:

    // Set the matrix dimensions
    void setDimensions(int newDim);
    void setDimensions(int newDim1, int newDim2);

    // Generate the matrix
    void renderMatrix() noexcept;

    //==============================================

    int dim1;
    int dim2;
    Eigen::MatrixXf matrix;

};

static_assert(std::is_copy_constructible_v<Matrix>,
    "Matrix must be copyable");
static_assert(std::is_move_constructible_v<Matrix>,
    "Matrix must be movable");
static_assert(std::is_nothrow_move_assignable_v<Matrix>,
    "Matrix move assignment should not throw");

}
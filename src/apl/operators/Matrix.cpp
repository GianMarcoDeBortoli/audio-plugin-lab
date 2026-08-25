#include <cmath> 
#include <cassert>

#include "operators/Matrix.h"

namespace apl::operators
{

Matrix::Matrix(int initDim) :
    dim1 { initDim },
    dim2 { initDim }
{
    assert( initDim >= 0
        && "Matrix dimension must be greater than or equal to zero" );

    matrix = Matrix::renderMatrix();
}

Matrix::Matrix(int initDim1, int initDim2) :
    dim1 { initDim1 },
    dim2 { initDim2 }
{
    assert( initDim1 >= 0 && initDim2 >= 0
        && "Matrix dimensions must be greater than or equal to zero" );

    matrix = Matrix::renderMatrix();
}

//================================================
// SET METHODS
void Matrix::setDimensions(int newDim) // private
{
    assert( newDim >= 0
        && "Matrix dimension must be greater than or equal to zero" );

    dim1 = newDim;
    dim2 = newDim;
}

void Matrix::setDimensions(int newDim1, int newDim2) // private
{
    assert( newDim1 >= 0 && newDim2 >= 0
        && "Matrix dimensions must be greater than or equal to zero" );

    dim1 = newDim1;
    dim2 = newDim2;
}

// STATE METHODS
Eigen::MatrixXf Matrix::renderMatrix() // private
{
    int maxDim = std::max(dim1, dim2);
    // Generate a random matrix with specified dimensions
    Eigen::MatrixXf random = Eigen::MatrixXf::Random(maxDim, maxDim);
    // Perform QR decomposition to obtain an orthogonal matrix
    Eigen::HouseholderQR<Eigen::MatrixXf> qr(random);
    // Set the matrix to the orthogonal matrix obtained from QR decomposition
    Eigen::MatrixXf coupling =
        Eigen::MatrixXf::Identity(dim1, maxDim)
        * qr.householderQ()
        * Eigen::MatrixXf::Identity(maxDim, dim2);

    // Scale matrix for expected-power preservation
    if ( dim1 < dim2 )
    {
        float normValue = std::sqrt(dim2 / dim1);
        coupling = normValue * coupling;
    }

    return coupling;
}

void Matrix::prepare(int newDim)
{   
    if (newDim != dim1 || newDim != dim2)
    {
        Matrix::setDimensions(newDim);
        Matrix::renderMatrix();
    }
}

void Matrix::prepare(int newDim1, int newDim2)
{
    if (newDim1 != dim1 || newDim2 != dim2)
    {
        Matrix::setDimensions(newDim1, newDim2);
        Matrix::renderMatrix();
    }
}

//================================================
// PROCESS METHODS
void Matrix::processSample(
    float* outSamples,
    const float* inSamples,
    uint32_t numOutputChannels,
    uint32_t numInputChannels
)
{
    assert( numInputChannels == dim2
        && "Number of channels must match the matrix dimension" );
    assert( numOutputChannels == dim1
        && "Number of channels must match the matrix dimension" );

    // Map the input samples to an Eigen matrix
    Eigen::Map<const Eigen::VectorXf> input(inSamples, numInputChannels);
    // Map the output samples to an Eigen matrix
    Eigen::Map<Eigen::VectorXf> output(outSamples, numOutputChannels);
    // Perform matrix multiplication
    output = matrix * input;
}

}
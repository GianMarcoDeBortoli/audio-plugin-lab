#include "Matrix.h"
#include <cstdint>

namespace DSP
{

Matrix::Matrix(int initDim)
{
    jassert(initDim >= 0 && "Matrix dimension must be greater than or equal to zero");
    dim1 = initDim;
    dim2 = initDim;
    matrix = genRandomOrthogonal(dim1);
}

Matrix::Matrix(int initDim1, int initDim2)
{
    jassert(initDim1 >= 0 && initDim2 >= 0 && "Matrix dimensions must be greater than or equal to zero");
    dim1 = initDim1;
    dim2 = initDim2;
    matrix = genRandomCoupling(dim1, dim2);
}

Matrix::~Matrix()
{
}

Eigen::MatrixXf Matrix::genRandomOrthogonal(int dim)
{
    // Generate a random orthogonal square matrix
    Eigen::MatrixXf random = Eigen::MatrixXf::Random(dim, dim);
    // Perform QR decomposition to obtain an orthogonal matrix
    Eigen::HouseholderQR<Eigen::MatrixXf> qr(random);
    // Set the matrix to the orthogonal matrix obtained from QR decomposition
    return qr.householderQ();
}

Eigen::MatrixXf Matrix::genRandomCoupling(int dim1, int dim2)
{
    int maxDim = std::max(dim1, dim2);
    // Generate a random matrix with specified dimensions
    Eigen::MatrixXf random = Eigen::MatrixXf::Random(maxDim, maxDim);
    // Perform QR decomposition to obtain an orthogonal matrix
    Eigen::HouseholderQR<Eigen::MatrixXf> qr(random);
    // Set the matrix to the orthogonal matrix obtained from QR decomposition
    return Eigen::MatrixXf::Identity(dim1, maxDim) * qr.householderQ() * Eigen::MatrixXf::Identity(maxDim, dim2);
}

void Matrix::setDimensions(int newDim)
{
    jassert(newDim >= 0 && "Matrix dimension must be greater than or equal to zero");
    dim1 = newDim;
    dim2 = newDim;
    matrix = genRandomOrthogonal(dim1);
}

void Matrix::setDimensions(int newDim1, int newDim2)
{
    jassert(newDim1 >= 0 && newDim2 >= 0 && "Matrix dimensions must be greater than or equal to zero");
    dim1 = newDim1;
    dim2 = newDim2;
    matrix = genRandomCoupling(dim1, dim2);
}

void Matrix::prepare(int newDim)
{   
    if (newDim != dim1 || newDim != dim2)
    {
        dim1 = newDim;
        dim2 = newDim;
        setDimensions(dim1);
    }
}

void Matrix::prepare(int newDim1, int newDim2)
{
    if (newDim1 != dim1 || newDim2 != dim2)
    {
        dim1 = newDim1;
        dim2 = newDim2;
        setDimensions(newDim1, newDim2);
    }
}

void Matrix::clear()
{
}

void Matrix::processSample(float* outSamples, const float* inSamples, uint32_t numOutputChannels, uint32_t numInputChannels)
{
    jassert(numInputChannels == dim2 && "Number of channels must match the matrix dimension");
    jassert(numOutputChannels == dim1 && "Number of channels must match the matrix dimension");

    // Map the input samples to an Eigen matrix
    Eigen::Map<const Eigen::VectorXf> input(inSamples, numInputChannels);
    // Map the output samples to an Eigen matrix
    Eigen::Map<Eigen::VectorXf> output(outSamples, numOutputChannels);
    // Perform matrix multiplication
    output = matrix * input;
}


}
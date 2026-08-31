#include <stdexcept>
#include <cassert>
#include <juce_dsp/juce_dsp.h>

#include "dsp/FFT.h"

namespace apl::dsp
{

class FFT::fftImplementation
{
public:
    // Constructor
    fftImplementation() = delete;
    explicit fftImplementation(std::size_t order) :
        fft(static_cast<int>(order))
    {
    }
    // Destructor
    ~fftImplementation() = default;

    // Copy
    fftImplementation(const fftImplementation&) = delete;
    fftImplementation& operator=(const fftImplementation&) = delete;

    // Move
    fftImplementation(fftImplementation&&) noexcept = default;
    fftImplementation& operator=(fftImplementation&&) noexcept = default;

    // Wrapper of JUCE fft
    juce::dsp::FFT fft;

    // Non-negative-frequency bins are stored as interleaved
    // real/imaginary pairs:
    //
    // data[2*k]     = real component of bin k
    // data[2*k + 1] = imaginary component of bin k
    //
    // for 0 <= k <= N/2.
    //
    // DC (k = 0) and Nyquist (k = N/2) have zero imaginary parts.
    //
    // The underlying JUCE FFT operates on N real samples and
    // requires a buffer of 2*N floats.

};

//================================================

FFT::FFT(std::size_t order) : 
    fftEngine { nullptr },
    fftSize { 0 },
    bufferSize { 0 }
{
    if (order == 0)
        throw std::invalid_argument("FFT order must be greater than zero");
    // Prevent an invalid shift / unreasonable FFT size.
    if (order >= sizeof(std::size_t) * 8 - 1)
        throw std::invalid_argument("FFT order is too large");

    fftSize = std::size_t{1} << order;
    bufferSize = std::size_t{1} << (order+1);
    fftEngine = std::make_unique<fftImplementation>(order);
}

FFT::~FFT() = default;

//================================================
// SET METHODS
void FFT::setBinValue(float* data, const std::size_t dataSize, std::size_t k, float realPart, float imagPart) const
{
    if (data == nullptr)
        throw std::invalid_argument("FFT::setBin received a null pointer");
    if (dataSize != bufferSize)
        throw std::invalid_argument("Data size does not match FFT size");
    if (k >= FFT::getNumBins())
        throw std::out_of_range("FFT frequency bin out of range");

    // DC and Nyquist are constrained to be real.
    if ((k == 0 || k == fftSize/2) && imagPart != 0)
        throw std::invalid_argument("DC and Nyquist frequency bins must remain real");

    data[2 * k] = realPart;
    data[2 * k + 1] = imagPart;
}

// GET METHODS
std::size_t FFT::getNfft() const noexcept
{
    return fftSize;
}

std::size_t FFT::getNumBins() const noexcept
{
    return fftSize / 2 + 1;
}

std::size_t FFT::getBufferSize() const noexcept
{
    return bufferSize;
}

float FFT::getBinReal(const float* data, const std::size_t dataSize, std::size_t k) const
{
    assert( data != nullptr
        && "FFT::getBinReal received a null pointer" );
    if (dataSize != bufferSize)
        throw std::invalid_argument("Data size does not match FFT size");
    if (k >= getNumBins())
        throw std::out_of_range("FFT frequency bin out of range");

    return data[2 * k];
}

float FFT::getBinImag(const float* data, const std::size_t dataSize, std::size_t k) const
{
    assert( data != nullptr
        && "FFT::getBinImag received a null pointer" );
    if (dataSize != bufferSize)
        throw std::invalid_argument("Data size does not match FFT size");
    if (k >= getNumBins())
        throw std::out_of_range("FFT frequency bin out of range");

    return data[2 * k + 1];
}

//================================================
// PROCESS METHODS
void FFT::rfft(float* data, const std::size_t dataSize) const
{
    assert( data != nullptr
        && "FFT::rfft received a null pointer" );
    if (dataSize != bufferSize)
        throw std::invalid_argument("Data size does not match FFT size");

    fftEngine->fft.performRealOnlyForwardTransform(data, true);
}

void FFT::irfft(float* data, const std::size_t dataSize) const
{
    assert( data != nullptr
        && "FFT::irfft received a null pointer" );
    if (dataSize != bufferSize)
        throw std::invalid_argument("Data size does not match FFT size");

    fftEngine->fft.performRealOnlyInverseTransform(data);
}

}
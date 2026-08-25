#include <stdexcept>
#include <cassert>
#include <juce_dsp/juce_dsp.h>

#include "dsp/FFT.h"

namespace apl::dsp
{

class FFT::fftImplementation
{
public:
    explicit fftImplementation(std::size_t order)
        : fft(static_cast<int>(order))
    {
    }

    juce::dsp::FFT fft;
};

FFT::FFT(std::size_t order) : 
    fftEngine(nullptr),
    fftSize(0)
{
    if (order == 0)
        throw std::invalid_argument("FFT order must be greater than zero");
    // Prevent an invalid shift / unreasonable FFT size.
    if (order >= sizeof(std::size_t) * 8)
        throw std::invalid_argument("FFT order is too large");

    fftSize = std::size_t{1} << order;
    fftEngine = std::make_unique<fftImplementation>(order);
}

//================================================
// SET METHODS
void FFT::setBinValue(float* data, std::size_t k, float realPart, float imagPart) const
{
    if (data == nullptr)
        throw std::invalid_argument("FFT::setBin received a null pointer");
    if (k >= FFT::getNumBins())
        throw std::out_of_range("FFT frequency bin out of range");

    // DC and Nyquist are constrained to be real.
    if (k == 0)
        data[0] = realPart; return;

    if (k == fftSize / 2)
        data[1] = realPart; return;

    data[2 * k] = realPart;
    data[2 * k + 1] = imagPart;
}

// GET METHODS
std::size_t FFT::getSize() const noexcept
{
    return fftSize;
}

std::size_t FFT::getNumBins() const noexcept
{
    return fftSize / 2 + 1;
}

float FFT::getBinReal(const float* data, std::size_t k) const
{
    assert( data != nullptr
        && "FFT::getBinReal received a null pointer" );
    if (k >= getNumBins())
        throw std::out_of_range("FFT frequency bin out of range");

    // JUCE's packed real FFT representation - interleaved:
    //
    // data[0] = DC real component
    // data[1] = Nyquist real component
    //
    // data[2*k]     = real component of bin k
    // data[2*k + 1] = imaginary component of bin k
    //
    // for 1 <= k < N/2.

    if (k == 0)
        return data[0];

    if (k == fftSize / 2)
        return data[1];

    return data[2 * k];
}

float FFT::getBinImag(const float* data, std::size_t k) const
{
    assert( data != nullptr
        && "FFT::getBinImag received a null pointer" );
    if (k >= getNumBins())
        throw std::out_of_range("FFT frequency bin out of range");

    // DC and Nyquist are purely real.
    if (k == 0 || k == fftSize / 2)
        return 0.0f;

    return data[2 * k + 1];
}

//================================================
// PROCESS METHODS
void FFT::rfft(float* data)
{
    assert( data != nullptr
        && "FFT::rfft received a null pointer" );

    fftEngine->fft.performRealOnlyForwardTransform(data);
}

void FFT::irfft(float* data)
{
    assert( data != nullptr
        && "FFT::irfft received a null pointer" );

    fftEngine->fft.performRealOnlyInverseTransform(data);
}

}
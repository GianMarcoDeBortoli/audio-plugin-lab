#pragma once

#include <cstddef>
#include <memory>

namespace apl::dsp
{

class FFT
{
public:
    // Constructor
    FFT() = delete;
    explicit FFT(std::size_t order);

    // Destructor -> default

    // Copy
    FFT(const FFT&) = delete;
    FFT& operator=(const FFT&) = delete;

    // Move
    FFT(FFT&&) noexcept;
    FFT& operator=(FFT&&) noexcept;

    // =============================================
    // SET METHODS
    void setBinValue(float* data, std::size_t k, float real, float imag) const;

    // GET METHODS
    std::size_t getSize() const noexcept;
    std::size_t getNumBins() const noexcept; // = size / 2 + 1

    // Access only desired frequency bin
    float getBinReal(const float* data, std::size_t k) const;
    float getBinImag(const float* data, std::size_t k) const;

    // =============================================
    // PROCESS METHODS
    void rfft(float* data); // No normalisation
    void irfft(float* data); // Full normalization

private:

    class fftImplementation;
    std::unique_ptr<fftImplementation> fftEngine;
    std::size_t fftSize;

};

}
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

    // Destructor
    ~FFT();

    // Copy
    FFT(const FFT&) = delete;
    FFT& operator=(const FFT&) = delete;

    // Move
    FFT(FFT&&) noexcept = default;
    FFT& operator=(FFT&&) noexcept = default;

    // =============================================
    // SET METHODS
    void setBinValue(float* data, const std::size_t dataSize, std::size_t k, float real, float imag) const;

    // GET METHODS
    std::size_t getNfft() const noexcept;
    std::size_t getNumBins() const noexcept; // = size / 2 + 1
    std::size_t getBufferSize() const noexcept;

    // Access only desired frequency bin
    float getBinReal(const float* data, const std::size_t dataSize, std::size_t k) const;
    float getBinImag(const float* data, const std::size_t dataSize, std::size_t k) const;

    // =============================================
    // PROCESS METHODS
    void rfft(float* data, const std::size_t dataSize) const; // No normalisation
    void irfft(float* data, const std::size_t dataSize) const; // Full normalization

private:

    class fftImplementation;
    std::unique_ptr<fftImplementation> fftEngine;
    std::size_t fftSize;
    std::size_t bufferSize;

};


static_assert(!std::is_copy_constructible_v<FFT>,
    "FFT must not be copyable");
static_assert(!std::is_copy_assignable_v<FFT>,
    "FFT must not be copy assignable");
static_assert(std::is_nothrow_move_constructible_v<FFT>,
    "FFT move construction should not throw");
static_assert(std::is_move_constructible_v<FFT>,
    "FFT must be movable");
static_assert(std::is_nothrow_move_assignable_v<FFT>,
    "FFT move assignment should not throw");

}
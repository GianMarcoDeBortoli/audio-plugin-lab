#pragma once

#include "operators/TimeVaryingMatrix.h"
#include "utils/SmoothParameter.h"

namespace apl::operators
{

class FirstOrderShelf
{

public:

    // Constructor
    FirstOrderShelf() = delete;
    FirstOrderShelf(
        float initHDC,
        float initHNY,
        float initCrossFreq
    );

    // Destructor
    ~FirstOrderShelf() = default;

    // Copy
    FirstOrderShelf(const FirstOrderShelf&) = default;
    FirstOrderShelf& operator=(const FirstOrderShelf&) = default;

    // Move
    FirstOrderShelf(FirstOrderShelf&&) noexcept = default;
    FirstOrderShelf& operator=(FirstOrderShelf&&) noexcept = default;

    // =============================================
    // SET METHODS
    // Set new magnitude values
    void setDCMagnitudeValue(float newHDC);
    void setNYMagnitudeValue(float newHNY);
    // Set new crossover frequency
    void setCrossoverFrequency(float newCrossFreq);

    // =============================================
    // STATE METHODS
    // Clear content of internal buffer
    void clear() noexcept;
    // Prepare filter for processing
    void prepare() noexcept;

    // =============================================
    // Process audio sample - single channel
    float processSample(float inSample) noexcept;
    // Process audio block - single channel
    void processBlock(float* outBlock, const float* inBlock, uint32_t numSamples) noexcept;

    // =============================================

private:

    // Compute the filter coefficients
    void computeCoefficients() noexcept;

    // =============================================
    // Filter coefficients
    apl::utils::SmoothParameter b0;
    apl::utils::SmoothParameter b1;
    apl::utils::SmoothParameter a1;
    // State
    float HDC;
    float HNY;
    float crossAngularFreq;
    float prevInput;
    float prevOutput;

    // =============================================
    static constexpr float pi = apl::math::constants::pi;
    
};

static_assert(std::is_copy_constructible_v<FirstOrderShelf>,
    "FirstOrderShelf must be copiable");
static_assert(std::is_move_constructible_v<FirstOrderShelf>,
    "FirstOrderShelf must be movable");
static_assert(std::is_nothrow_move_assignable_v<FirstOrderShelf>,
    "Move assingment should not throw");

}
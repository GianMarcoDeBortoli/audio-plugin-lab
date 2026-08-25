#pragma once

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

    // Destructor -> default

    // Copy
    FirstOrderShelf(const FirstOrderShelf&);
    const FirstOrderShelf& operator=(const FirstOrderShelf&);

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
    void clear();
    // Prepare filter for processing
    void prepare();

    // =============================================
    // Process audio sample - single channel
    float processSample(float inSample);
    // Process audio block - single channel
    void processBlock(float* outBlock, const float* inBlock, uint32_t numSamples);

    // =============================================

private:

    // Compute the filter coefficients
    void computeCoefficients();

    // =============================================

    // Filter coefficients
    apl::utils::SmoothParameter b0;
    apl::utils::SmoothParameter b1;
    apl::utils::SmoothParameter a1;
    // State
    float HDC;
    float HNY;
    float crossFreq;
    float prevInput;
    float prevOutput;
    
};

static_assert(std::is_copy_constructible_v<FirstOrderShelf>, "FirstOrderShelf must be copiable");
static_assert(std::is_move_constructible_v<FirstOrderShelf>, "FirstOrderShelf must be movable");
static_assert(std::is_nothrow_move_assignable_v<FirstOrderShelf>, "Move assingment should not throw");

}
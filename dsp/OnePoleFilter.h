#pragma once

#include "Ramp.h"

namespace DSP
{

class OnePoleFilter
{
public:
    // Constructor
    OnePoleFilter(
        float initMagDC,
        float initMagNY
    );
    // Destructor
    ~OnePoleFilter();

    // No copy semantics
    OnePoleFilter(const OnePoleFilter&);
    const OnePoleFilter& operator=(const OnePoleFilter&);
    // No move semantics
    OnePoleFilter(OnePoleFilter&&) noexcept = default;
    OnePoleFilter& operator=(OnePoleFilter&&) noexcept = default;

    // =============================================

    // Compute the coefficients
    void computeCoefficients();

    // Set new coefficients
    void setMagValues(float newMagDC, float newMagNY);

    // =============================================

    // Prepare coefficient ramp
    void prepare(double newSampleRate, int samplesPerBlock);

    // Clear content of internal buffer
    void clear();

    // =============================================
    // Process single-channel sample
    void processSample(float* output, const float* input);

    // Process single-channel buffer
    void processBuffer(float* output, const float* input, uint32_t numSamples);

    // =============================================

private:
    // Magnitude at DC and Nyquist
    float currentMagDC;
    float currentMagNY;
    // Filter coefficients
    DSP::Ramp b0Ramp;
    float b0;
    DSP::Ramp a1Ramp;
    float a1;

    // one state per channel
    float feedbackState;

    // static_assert(std::is_copy_constructible_v<OnePoleFilter>);
    // static_assert(std::is_move_constructible_v<OnePoleFilter>);
    static_assert(std::is_nothrow_move_assignable_v<OnePoleFilter>);
};

}
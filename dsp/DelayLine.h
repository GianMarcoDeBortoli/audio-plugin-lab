#pragma once

#include <JuceHeader.h>

#include "Ramp.h"

namespace DSP
{

class DelayLine
{
public:
    DelayLine(
        size_t maxDelaySamples,
        size_t initDelaySamples
    );
    ~DelayLine();

    // No default ctorx
    DelayLine() = delete;

    // No copy semantics
    DelayLine(const DelayLine&) = delete;
    const DelayLine& operator=(const DelayLine&) = delete;

    // Allow move semantics
    DelayLine(DelayLine&&) noexcept = default;
    DelayLine& operator=(DelayLine&&) noexcept = default;

    // =============================================

    // Set the current delay time
    void setDelay(size_t newDelaySamples);

    // =============================================

    // Prepare the delay line for processing
    void prepare(double newSampleRate, int samplesPerBlock);

    // Clear the content of the delay buffer
    void clear();

    // Fixed delay length
    // Process single-channel sample
    void processSample(float* outSample, const float* inSample);

    // Modulated delay length (linear interpolation)
    // Process single-channel sample
    void processSample(float* outSample, const float* inSample, const float* modInput);

private:
    size_t delayBufferSize;
    std::vector<float> delayBuffer;

    size_t currentDelaySamples;
    DSP::Ramp delayRamp;

    size_t currentWriteIndex;

    // static_assert(std::is_copy_constructible_v<DelayLine>);
    // static_assert(std::is_move_constructible_v<DelayLine>);
    static_assert(std::is_nothrow_move_assignable_v<DelayLine>);
};

}
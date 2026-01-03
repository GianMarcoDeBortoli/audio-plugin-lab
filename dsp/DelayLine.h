#pragma once

#include <vector>

#include "SmoothParameter.h"

namespace primitive
{

class DelayLine
{
public:
    DelayLine(
        uint32_t maxDelaySamples,
        uint32_t initDelaySamples
    );
    ~DelayLine();

    // No default constructor
    DelayLine() = delete;

    // No copy semantics
    DelayLine(const DelayLine&) = delete;
    const DelayLine& operator=(const DelayLine&) = delete;

    // Allow move semantics
    DelayLine(DelayLine&&) noexcept = default;
    DelayLine& operator=(DelayLine&&) noexcept = default;

    // =============================================

    // Set the current delay time
    void setDelay(uint32_t newDelaySamples);

    // =============================================

    // Prepare the delay line for processing
    void prepare();

    // Clear the content of the delay buffer
    void clear();

    // =============================================

    // Process audio sample - linear interpolation only
    void processSample(float* outSample, const float* inSample, float modInput = 0.0f);

    // Process block of audio - wrapper of processSample
    void processBlock(float* outBlock, const float* inBlock, uint32_t numSamples, const float* modInput = nullptr);

private:
    size_t delayBufferSize;
    std::vector<float> delayBuffer;

    utils::SmoothParameter delayValue;

    size_t writeIndex;

    // static_assert(std::is_copy_constructible_v<DelayLine>);
    // static_assert(std::is_move_constructible_v<DelayLine>);
    static_assert(std::is_nothrow_move_assignable_v<DelayLine>);
};

}
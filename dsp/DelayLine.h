#pragma once

#include <type_traits>
#include <vector>

#include "SmoothParameter.h"

namespace primitives
{

class DelayLine
{
public:

    // Constructor
    DelayLine() = delete;
    DelayLine(
        uint32_t maxDelaySamples,
        uint32_t initDelaySamples
    );

    // Destructor -> default

    // Copy
    DelayLine(const DelayLine&) = delete;
    DelayLine& operator=(const DelayLine&) = delete;

    // Move
    DelayLine(DelayLine&&) noexcept = default;
    DelayLine& operator=(DelayLine&&) noexcept = default;

    //================================================

    // Set the current delay time
    void setDelay(uint32_t newDelaySamples);

    //================================================

    // Prepare the delay line for processing
    void prepare();

    // Clear the content of the delay buffer
    void clear();

    //================================================

    // Process audio sample - linear interpolation only
    void processSample(float* outSample, const float* inSample, float modInput = 0.0f);

    // Process block of audio - wrapper of processSample
    void processBlock(float* outBlock, const float* inBlock, uint32_t numSamples, const float* modInput = nullptr);

    //================================================

private:

    utils::SmoothParameter delayValue;
    size_t delayBufferSize;
    std::vector<float> delayBuffer;
    size_t writeIndex;

    //================================================

    static_assert(std::is_move_constructible_v<DelayLine>, "DelayLine must be movable");
    static_assert(std::is_nothrow_move_assignable_v<DelayLine>, "Move assignment should not throw");
};

}
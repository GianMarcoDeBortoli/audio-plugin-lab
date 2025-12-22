#pragma once

#include <cstdint>

#include <JuceHeader.h>

#include "DelayLine.h"

namespace DSP
{

class MultichannelDelay
{
public:
    MultichannelDelay(
        uint32_t initDelayLinesNumber,
        const std::vector<size_t>& initDelayLinesMaxLengths,
        const std::vector<size_t>& initDelayLengths
    );
    ~MultichannelDelay();

    // No default ctor
    MultichannelDelay() = delete;

    // No copy semantics
    MultichannelDelay(const MultichannelDelay&) = delete;
    const MultichannelDelay& operator=(const MultichannelDelay&) = delete;

    // No move semantics
    MultichannelDelay(MultichannelDelay&&) noexcept = default;
    MultichannelDelay& operator=(MultichannelDelay&&) noexcept = default;

    // =============================================

    // Set the delay time in samples of the delay lines
    void setDelayLinesLengths(const std::vector<size_t>& newDelayLinesLengths);

    // =============================================

    // Prepare the delay lines for processing
    void prepare(double newSampleRate, int samplesPerBlock);

    // Clear the contents of the delay buffer
    void clear();

    // Fixed delay length
    // Process multi-channel sample
    void processSample(float* outSamples, const float* inSamples, uint32_t numChannels);

    // Modulated delay length (linear interpolation)
    // Process multi-channel sample
    void processSample(float* outSamples, const float* inSamples, const float* modInput, uint32_t numChannels);

private:
    double sampleRate { 48000.0 };

    uint32_t delayLinesNumber;
    std::vector<DSP::DelayLine> delayLines;

    // static_assert(std::is_copy_constructible_v<MultichannelDelay>);
    // static_assert(std::is_move_constructible_v<MultichannelDelay>);
    static_assert(std::is_nothrow_move_assignable_v<MultichannelDelay>);
};

}
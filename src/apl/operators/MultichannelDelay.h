#pragma once

#include <cstdint>

#include "operators/DelayLine.h"

namespace apl::operators
{

class MultichannelDelay
{

public:

    // Constructor
    MultichannelDelay() = delete;
    MultichannelDelay(
        uint32_t initChannels,
        const std::vector<size_t>& initDelayLinesMaxLengths,
        const std::vector<float>& initDelayLengths
    );

    // Destructor -> default

    // Copy
    MultichannelDelay(const MultichannelDelay&) = delete;
    const MultichannelDelay& operator=(const MultichannelDelay&) = delete;

    // Move
    MultichannelDelay(MultichannelDelay&&) noexcept = default;
    MultichannelDelay& operator=(MultichannelDelay&&) noexcept = default;

    // =============================================
    // SET METHODS
    // Set the delay time in samples of the delay lines
    void setDelayLinesLengths(const std::vector<float>& newDelayLinesLengths);

    //================================================
    // STATE METHODS
    // Clear the contents of the delay buffer
    void clear();
    // Prepare the delay lines for processing
    void prepare();

    //==============================================
    // PROCESS METHODS
    // Process audio sample - multichannel
    void processSample(float* outSamples, const float* inSamples, uint32_t numChannels);

    //================================================

private:

    uint32_t channels;
    std::vector<apl::operators::DelayLine> delayLines;

};

// static_assert(std::is_copy_constructible_v<MultichannelDelay>);
// static_assert(std::is_move_constructible_v<MultichannelDelay>);
static_assert(std::is_nothrow_move_assignable_v<MultichannelDelay>);

}
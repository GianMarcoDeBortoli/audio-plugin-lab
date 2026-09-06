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
        const std::vector<float>& initDelayLinesLengths
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
    // Set the delay lengths of the delay lines in samples
    void setDelayLinesLengths(const std::vector<float>& newDelayLinesLengths);

    // GET METHODS
    // Get the delay lengths of the delay lines in samples
    std::vector<float> getDelayLinesLengths() const;

    //================================================
    // STATE METHODS
    // Clear the contents of the delay buffer
    void clear();
    // Prepare the delay lines for processing
    void prepare();

    //==============================================
    // PROCESS METHODS
    // Get the sample at the current delay value
    void getSample(float* outSample, uint32_t numChannels);
    // Set the sample at the current write pointer
    void setSample(const float* inSample, uint32_t numChannels);
    // Advance write pointer
    void advancePointer(uint32_t numChannels);

    // Process audio sample - multichannel
    void processSample(float* outSample, const float* inSample, uint32_t numChannels);

    //================================================

private:

    uint32_t channels;
    std::vector<apl::operators::DelayLine> delayLines;

};

// static_assert(std::is_copy_constructible_v<MultichannelDelay>);
// static_assert(std::is_move_constructible_v<MultichannelDelay>);
static_assert(std::is_nothrow_move_assignable_v<MultichannelDelay>);

}
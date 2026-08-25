#pragma once

#include <cstdint>
#include <vector>

#include "operators/FirstOrderShelf.h"

namespace apl::operators
{

class MultichannelAbsorption
{

public:

    // Constructor
    MultichannelAbsorption() = delete;
    MultichannelAbsorption(
        uint32_t initChannels,
        const std::vector<float> initHDC,
        const std::vector<float> initHNY,
        float initCrossFreq
    );

    // Destructor -> default

    // Copy
    MultichannelAbsorption(const MultichannelAbsorption&) = delete;
    const MultichannelAbsorption& operator=(const MultichannelAbsorption&) = delete;

    // Move
    MultichannelAbsorption(MultichannelAbsorption&&) noexcept = default;
    MultichannelAbsorption& operator=(MultichannelAbsorption&&) noexcept = default;

    // =============================================
    // SET METHODS
    // Set the magnitude values
    void setDCMagnitudeValue(const std::vector<float> newHCD);
    void setNYMagnitudeValue(const std::vector<float> newHNY);
    // Set the crossover frequency value
    void setCrossFrequency(float crossFreq);

    //================================================
    // STATE METHODS
    // Clear the contents of the filter states
    void clear();
    // Prepare the filters for processing
    void prepare();

    // =============================================
    // PROCESS METHODS 
    // Process audio sample - multichannel
    void processSample(float* outSamples, const float* inSamples, uint32_t numChannels);

    //================================================

private:

    uint32_t channels;
    std::vector<apl::operators::FirstOrderShelf> filters;

};

// static_assert(std::is_copy_constructible_v<MultichannelAbsorption>);
// static_assert(std::is_move_constructible_v<MultichannelAbsorption>);
static_assert(std::is_nothrow_move_assignable_v<MultichannelAbsorption>);

}
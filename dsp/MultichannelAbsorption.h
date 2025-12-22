#pragma once

#include <cstdint>

#include <JuceHeader.h>

#include "OnePoleFilter.h"

namespace DSP
{

class MultichannelAbsorption
{
public:
    MultichannelAbsorption(
        uint32_t initFiltersNumber,
        const std::vector<std::pair<float, float>>& initFiltersMagValues
    );
    ~MultichannelAbsorption();

    // No default ctor
    MultichannelAbsorption() = delete;

    // No copy semantics
    MultichannelAbsorption(const MultichannelAbsorption&) = delete;
    const MultichannelAbsorption& operator=(const MultichannelAbsorption&) = delete;

    // No move semantics
    MultichannelAbsorption(MultichannelAbsorption&&) noexcept = default;
    MultichannelAbsorption& operator=(MultichannelAbsorption&&) noexcept = default;

    // =============================================

    // Set the filter coefficients (SOS) for the filters
    void setFiltersMagnitudeValues(const std::vector<std::pair<float, float>>& newFiltersMagValues);

    // =============================================

    // Prepare the filters for processing
    void prepare(double newSampleRate, int samplesPerBlock);

    // Clear the contents of the filter states
    void clear();

    // Process multi-channel sample
    void processSample(float* outSamples, const float* inSamples, uint32_t numChannels);

private:
    double sampleRate { 48000.0 };

    uint32_t filtersNumber;
    std::vector<DSP::OnePoleFilter> filters;

    // static_assert(std::is_copy_constructible_v<MultichannelAbsorption>);
    // static_assert(std::is_move_constructible_v<MultichannelAbsorption>);
    static_assert(std::is_nothrow_move_assignable_v<MultichannelAbsorption>);
};

}
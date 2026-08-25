#include <cassert>

#include "operators/MultichannelAbsorption.h"

namespace apl::operators
{
    

MultichannelAbsorption::MultichannelAbsorption(
    uint32_t initChannels,
    const std::vector<float> initHDC,
    const std::vector<float> initHNY,
    float initCrossFreq
)
{
    // Check if the number of filters is valid
    assert( initChannels > 0u && "Number of channels must be greater than zero" );
    channels = initChannels;

    // Reserve memory for the vector of filters
    filters.reserve(static_cast<size_t>(channels));

    // Initialize each filter
    assert( initHDC.size() == static_cast<size_t>(channels) && initHNY.size() == static_cast<size_t>(channels)
        && "Filter magnitude values size must match the number of filters" );

    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        filters.emplace_back(initHDC[i], initHNY[i], initCrossFreq);
}

void MultichannelAbsorption::setDCMagnitudeValue(const std::vector<float> newHDC)
{
    assert( newHDC.size() == channels
        && "New filter magnitude values size must match the number of filters");

    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        filters[i].setDCMagnitudeValue(newHDC[i]);
}

void MultichannelAbsorption::setNYMagnitudeValue(const std::vector<float> newHNY)
{
    assert( newHNY.size() == channels
        && "New filter magnitude values size must match the number of filters");

    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        filters[i].setDCMagnitudeValue(newHNY[i]);
}

void MultichannelAbsorption::setCrossFrequency(float newCrossFreq)
{
    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        filters[i].setDCMagnitudeValue(newCrossFreq);
}

void MultichannelAbsorption::clear()
{
    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        filters[i].clear();
}

void MultichannelAbsorption::prepare()
{
    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        filters[i].prepare();
}

void MultichannelAbsorption::processSample(float* outSamples, const float* inSamples, uint32_t numChannels)
{
    assert( numChannels == channels 
        && "Number of channels must match the number of filters" );

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        outSamples[ch] = filters[ch].processSample(inSamples[ch]);
}

}
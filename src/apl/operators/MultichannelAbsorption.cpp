#include "MultichannelAbsorption.h"

namespace DSP
{

MultichannelAbsorption::MultichannelAbsorption(uint32_t initFiltersNumber, const std::vector<std::pair<float, float>>& initFiltersMagValues)
{
    // Check if the number of filters is valid
    jassert(initFiltersNumber > 0u && "Number of filters must be greater than zero");
    filtersNumber = initFiltersNumber;

    // Reserve memory for the vector of filters
    filters.reserve(static_cast<size_t>(filtersNumber));

    // Initialize each filter
    jassert(initFiltersMagValues.size() == static_cast<size_t>(filtersNumber) && "Filter magnitude values size must match the number of filters");
    for (size_t i = 0; i < static_cast<size_t>(filtersNumber); ++i)
        filters.emplace_back(initFiltersMagValues[i].first, initFiltersMagValues[i].second);
}

MultichannelAbsorption::~MultichannelAbsorption()
{
}

void MultichannelAbsorption::setFiltersMagnitudeValues(const std::vector<std::pair<float, float>>& newFiltersMagValues)
{
    jassert(newFiltersMagValues.size() == filtersNumber && "New filter magnitude values size must match the number of filters");
    for (size_t i = 0; i < static_cast<size_t>(filtersNumber); ++i)
        filters[i].setMagValues(newFiltersMagValues[i].first, newFiltersMagValues[i].second);
}

void MultichannelAbsorption::prepare(double newSampleRate, int samplesPerBlock)
{
    jassert(newSampleRate > 0.0 && "Sample rate must be greater than zero");
    sampleRate = newSampleRate;

    // Prepare each delay line for processing
    for (size_t i = 0; i < static_cast<size_t>(filtersNumber); ++i)
        filters[i].prepare(newSampleRate, samplesPerBlock);
}

void MultichannelAbsorption::clear()
{
    for (size_t i = 0; i < static_cast<size_t>(filtersNumber); ++i)
        filters[i].clear();
}

void MultichannelAbsorption::processSample(float* outSamples, const float* inSamples, uint32_t numChannels)
{
    jassert(numChannels == filtersNumber && "Number of channels must match the number of filters");

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        filters[ch].processSample(&outSamples[ch], &inSamples[ch]);
}

}
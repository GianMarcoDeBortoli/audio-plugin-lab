#include "FDN.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include <cstddef>

#include <random>

namespace DSP
{

FDN::FDN(uint32_t initOrder, float initT60DC, float initBrightness) :
    // Check if the order is valid
    order { checkOrder(initOrder) },
    // Initialize sample rate
    feedbackMatrix { static_cast<int>(order), static_cast<int>(order) }
{
    // Initialize delay lines
    delayLengths.reserve(order);
    delayLengths = computeDelayLengths();
    maxDelayLengths = computeMaxDelayLinesLengths();
    delayLines = std::make_unique<MultichannelDelay>(
        order,
        maxDelayLengths,
        delayLengths
    );

    // Initialize absorption filters
    jassert(initT60DC > 0.f && "T60 at DC must be greater than zero");
    jassert(initBrightness >= 0.f && initBrightness <= 1.f && "Brightness must be in [0, 1]");
    T60DC = initT60DC;
    brightness = initBrightness;
    absorptionMagnitudeValues = computeAbsorptionMagValues(T60DC, brightness, this->sampleRate);
    absorptionFilters = std::make_unique<MultichannelAbsorption>(
        order, 
        absorptionMagnitudeValues
    );

    // Initialize feedback state
    feedbackState.resize(order);
    std::fill(feedbackState.begin(), feedbackState.end(), 0.f);
}

FDN::~FDN()
{
}

uint32_t FDN::checkOrder(uint32_t order)
{
    for (const auto validOrder : possibleOrders)
    {
        if (order == validOrder)
            return order;
    }
    jassert(false && "Invalid FDN order");
    return 0u;
}

std::vector<size_t> FDN::computeDelayLengths()
{
    std::vector<size_t> delayLengths;
    delayLengths.reserve(this->order);

    // Define min and max delay length values (replace with your desired values)
    const size_t minDelay = 300; // example minimum
    const size_t maxDelay = 2600; // example maximum

    // Seed random generator (could use std::random_device for more randomness)
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> dist(minDelay, maxDelay);

    for (uint32_t i = 0; i < this->order; ++i)
    {
        delayLengths.push_back(dist(rng));
    }
    
    return delayLengths;
}

std::vector<size_t> FDN::computeMaxDelayLinesLengths(
    // TODO: Add time variation
)
{
    std::vector<size_t> maxDelayLengths;
    maxDelayLengths.reserve(this->order);

    // Calculate the maximum delay lengths for each filter
    for (uint32_t i = 0; i < this->order; ++i)
    {
        size_t maxLength = this->delayLengths[i] + 100;
        maxDelayLengths.push_back(maxLength);
    }

    return maxDelayLengths;
}

std::vector<std::pair<float, float>> FDN::computeAbsorptionMagValues(
    float T60DC,
    float brightness,
    double sampleRate
)
{
    std::vector<std::pair<float, float>> absorptionMagnitudeValues;
    absorptionMagnitudeValues.reserve(this->order);

    // Calculate the magnitude values for each filter
    for (uint32_t i = 0; i < this->order; ++i)
    {
        float magDCdB = static_cast<float>(this->delayLengths[i]) * ( -60.f / ( T60DC * static_cast<float>(sampleRate) )) ;
        float magDClinear = std::powf(10.f, magDCdB / 20.f);
        float T60Nyquist = T60DC * brightness;
        float magNYdB = static_cast<float>(this->delayLengths[i]) * ( -60.f / ( T60Nyquist * static_cast<float>(sampleRate) )) ;
        float magNYlinear = std::powf(10.f, magNYdB / 20.f);

        absorptionMagnitudeValues.emplace_back(magDClinear, magNYlinear);
    }

    return absorptionMagnitudeValues;
}

void FDN::setT60(float newT60DC)
{
    jassert(newT60DC > 0.f && "T60 at DC must be greater than zero");
    
    T60DC = newT60DC;

    // Update absorption filters with the new values
    absorptionMagnitudeValues = computeAbsorptionMagValues(T60DC, this->brightness, this->sampleRate);
    absorptionFilters->setFiltersMagnitudeValues(absorptionMagnitudeValues);
}

void FDN::setBrightness(float newBrightness)
{
    jassert(newBrightness >= 0.f && newBrightness <= 1.f && "Brightness must be in [0, 1]");
    
    brightness = newBrightness;

    // Update absorption filters with the new values
    absorptionMagnitudeValues = computeAbsorptionMagValues(this->T60DC, brightness, this->sampleRate);
    absorptionFilters->setFiltersMagnitudeValues(absorptionMagnitudeValues);
}

void FDN::prepare(double newSampleRate, int samplesPerBlock)
{
    jassert(newSampleRate > 0.0 && "Sample rate must be greater than zero");
    sampleRate = newSampleRate;

    // Prepare delay lines
    delayLines->prepare(this->sampleRate, samplesPerBlock);

    // Prepare absorption filters
    absorptionMagnitudeValues = computeAbsorptionMagValues(
        this->T60DC,
        this->brightness,
        this->sampleRate
    );
    absorptionFilters->setFiltersMagnitudeValues(absorptionMagnitudeValues);
    absorptionFilters->prepare(this->sampleRate, samplesPerBlock);
}

void FDN::clear()
{   
    // Clear fdn state
    std::fill(feedbackState.begin(), feedbackState.end(), 0.f);
    // Clear delay lines
    delayLines->clear();
    // Clear absorption filters
    absorptionFilters->clear();
}

void FDN::process(float* output, const float* input, uint32_t numChannels)
{
    jassert(numChannels == order && "Number of channels must match FDN order");

    for (uint32_t i = 0; i < numChannels; ++i)
    {
        feedbackState[i] += input[i];
    }

    delayLines->processSample(output, feedbackState.data(), order);
    absorptionFilters->processSample(output, output, order);
    feedbackMatrix.processSample(feedbackState.data(), output, order, order);
}

}
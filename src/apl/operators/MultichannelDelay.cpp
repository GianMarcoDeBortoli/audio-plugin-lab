#include <cassert>

#include "operators/MultichannelDelay.h"

namespace apl::operators
{

MultichannelDelay::MultichannelDelay(
    uint32_t initChannels,
    const std::vector<size_t>& initDelayLinesMaxLengths,
    const std::vector<float>& initDelayLinesLengths
)
{
    // Check if the number of delay lines is valid
    assert(initChannels > 0u
        && "Number of delay lines must be greater than zero");

    channels = initChannels;

    // Reserve memory for the vector of delay lines
    delayLines.reserve(static_cast<size_t>(channels));

    // Initialize each delay line
    assert(initDelayLinesMaxLengths.size() == static_cast<size_t>(channels)
        && "Delay-line-length size must match the number of delay lines");
    assert(initDelayLinesLengths.size() == static_cast<size_t>(channels)
        && "Initial delay lengths size must match the number of delay lines");

    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        delayLines.emplace_back(initDelayLinesMaxLengths[i], initDelayLinesLengths[i]);
}

// SET METHODS
void MultichannelDelay::setDelayLinesLengths(const std::vector<float>& newDelaysSamples)
{
    assert(newDelaysSamples.size() == channels
        && "New delay-line-length size must match the number of delay lines");

    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        delayLines[i].setDelayLength(newDelaysSamples[i]);
}

// GET METHODS
std::vector<float> MultichannelDelay::getDelayLinesLengths() const // std::vector<T>.reserve() might throw
{
    std::vector<float> delaySamples;
    delaySamples.reserve(channels);
    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        delaySamples.push_back(delayLines[i].getDelayLength());
    
    return delaySamples;
}

// STATE METHODS
void MultichannelDelay::prepare()
{
    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        delayLines[i].prepare();
}

void MultichannelDelay::clear()
{
    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        delayLines[i].clear();
}

// PROCESS METHODS
void MultichannelDelay::getSample(float* outSample, uint32_t numChannels)
{
    assert(outSample != nullptr);
    assert(numChannels == channels
        && "Number of channels must match the number of delay lines");

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(channels); ++ch)
        outSample[ch] = delayLines[ch].getSample();
}

void MultichannelDelay::setSample(const float* inSample, uint32_t numChannels)
{
    assert(inSample != nullptr);
    assert(numChannels == channels
        && "Number of channels must match the number of delay lines");

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(channels); ++ch)
        delayLines[ch].setSample(inSample[ch]);
}

void MultichannelDelay::advancePointer(uint32_t numChannels)
{
    assert(numChannels == channels
        && "Number of channels must match the number of delay lines");

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(channels); ++ch)
        delayLines[ch].advancePointer();
}

void MultichannelDelay::processSample(float* outSample, const float* inSample, uint32_t numChannels)
{   
    assert(outSample != nullptr);
    assert(inSample != nullptr);
    assert(numChannels == channels
        && "Number of channels must match the number of delay lines");

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(channels); ++ch)
        outSample[ch] = delayLines[ch].processSample(inSample[ch]);
}

}
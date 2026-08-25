#include <cassert>

#include "operators/MultichannelDelay.h"

namespace apl::operators
{

MultichannelDelay::MultichannelDelay(
    uint32_t initChannels,
    const std::vector<size_t>& initDelayLinesMaxLengths,
    const std::vector<size_t>& initDelayLengths
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
    assert(initDelayLengths.size() == static_cast<size_t>(channels)
        && "Initial delay lengths size must match the number of delay lines");

    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        delayLines.emplace_back(initDelayLinesMaxLengths[i], initDelayLengths[i]);
}

void MultichannelDelay::setDelayLinesLengths(const std::vector<size_t>& newDelaysSamples)
{
    assert(newDelaysSamples.size() == channels
        && "New delay-line-length size must match the number of delay lines");

    for (size_t i = 0; i < static_cast<size_t>(channels); ++i)
        delayLines[i].setDelayLength(newDelaysSamples[i]);
}

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

void MultichannelDelay::processSample(float* outSamples, const float* inSamples, uint32_t numChannels)
{
    assert(numChannels == channels
        && "Number of channels must match the number of delay lines");

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        outSamples[ch] = delayLines[ch].processSample(inSamples[ch]);
}

}
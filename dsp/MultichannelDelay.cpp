#include "MultichannelDelay.h"

namespace DSP
{

MultichannelDelay::MultichannelDelay(
    uint32_t initDelayLinesNumber,
    const std::vector<size_t>& initDelayLinesMaxLengths,
    const std::vector<size_t>& initDelayLengths
)
{
    // Check if the number of delay lines is valid
    jassert(initDelayLinesNumber > 0u && "Number of delay lines must be greater than zero");
    delayLinesNumber = initDelayLinesNumber;

    // Reserve memory for the vector of delay lines
    delayLines.reserve(static_cast<size_t>(delayLinesNumber));

    // Initialize each delay line
    jassert(initDelayLinesMaxLengths.size() == static_cast<size_t>(delayLinesNumber) && "Delay-line-length size must match the number of delay lines");
    jassert(initDelayLengths.size() == static_cast<size_t>(delayLinesNumber) && "Initial delay lengths size must match the number of delay lines");
    for (size_t i = 0; i < static_cast<size_t>(delayLinesNumber); ++i)
        delayLines.emplace_back(initDelayLinesMaxLengths[i], initDelayLengths[i]);
}

MultichannelDelay::~MultichannelDelay()
{
}

void MultichannelDelay::setDelayLinesLengths(const std::vector<size_t>& newDelaysSamples)
{
    jassert(newDelaysSamples.size() == delayLinesNumber && "New delay-line-length size must match the number of delay lines");
    for (size_t i = 0; i < static_cast<size_t>(delayLinesNumber); ++i)
        delayLines[i].setDelay(newDelaysSamples[i]);
}

void MultichannelDelay::prepare(double newSampleRate, int samplesPerBlock)
{
    jassert(newSampleRate > 0.0 && "Sample rate must be greater than zero");
    sampleRate = newSampleRate;

    // Prepare each delay line for processing
    for (size_t i = 0; i < static_cast<size_t>(delayLinesNumber); ++i)
        delayLines[i].prepare(newSampleRate, samplesPerBlock);
}

void MultichannelDelay::clear()
{
    for (size_t i = 0; i < static_cast<size_t>(delayLinesNumber); ++i)
        delayLines[i].clear();
}

void MultichannelDelay::processSample(float* outSamples, const float* inSamples, uint32_t numChannels)
{
    jassert(numChannels == delayLinesNumber && "Number of channels must match the number of delay lines");

    // Process each channel independently
    for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        delayLines[ch].processSample(&outSamples[ch], &inSamples[ch]);
}

void MultichannelDelay::processSample(float* outSamples, const float* inSamples, const float* modInput, uint32_t numChannels)
{
    jassert(numChannels == delayLinesNumber && "Number of channels must match the number of delay lines");

    // Process each channel independently with modulation
    for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        delayLines[ch].processSample(&outSamples[ch], &inSamples[ch], &modInput[ch]);
}

}
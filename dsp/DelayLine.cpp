#include <algorithm> 
#include <cmath>
#include <cassert>

#include "DelayLine.h"

namespace primitives
{

DelayLine::DelayLine(uint32_t maxDelaySamples, uint32_t initDelaySamples) :
    delayValue { static_cast<float>(initDelaySamples) }
{
    // Check if the maximum delay samples is valid
    assert(maxDelaySamples > 0 && "Maximum delay of the delay line must be greater than zero");
    // Add 1 sample to the buffer size to effectively use the requested maximum delay
    delayBufferSize = static_cast<size_t>(maxDelaySamples) + size_t { 1u };

    // Initialize the delay buffer with maximum delay size and fill it with zeros
    delayBuffer.resize(delayBufferSize, 0.f);

    // Initialize the current delay with a smoothing time to the requested value
    delayValue.setSmoothingTime(uint32_t { 1200u });
    assert(initDelaySamples > 0 && "Initial delay of the delay line must be greater than zero");
    assert(initDelaySamples <= maxDelaySamples && "Initial delay must be less than the maximum delay");
    delayValue.setTarget(static_cast<float>(initDelaySamples), true);

    // Initialize the current write index to zero
    writeIndex = size_t { 0u };
}

//================================================

void DelayLine::setDelay(uint32_t newDelaySamples)
{
    assert(static_cast<size_t>(newDelaySamples) <= delayBufferSize - size_t{ 1u } && "New delay must be less than the maximum delay");
    delayValue.setTarget(static_cast<float>(newDelaySamples), false);
}

//================================================

void DelayLine::prepare()
{
    delayValue.prepare();
    DelayLine::clear();
}

void DelayLine::clear()
{
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.f);
    writeIndex = size_t { 0u };
}

//================================================

void DelayLine::processSample(float* outSample, const float* inSample, float modInput /*= 0.0f*/)
{
    // Interpolate the read index for smooth ramping
    float delay = delayValue.getSample();
    delay += modInput;
    const float delayCeil  { std::ceil(delay) };
    const float delayFrac1 { delayCeil - delay };
    const float delayFrac0 {  1.f - delayFrac1 };

    // Calculate interpolation read indices based on the modulation value
    const size_t readIndex0 { (writeIndex + delayBufferSize - static_cast<size_t>(delayCeil)) % delayBufferSize };
    const size_t readIndex1 { (readIndex0 + delayBufferSize + static_cast<size_t>(    1u   )) % delayBufferSize };

    // Write input to the delay buffer
    delayBuffer.at(writeIndex) = *inSample;
    // Read output from the delay buffer
    const float read0 = delayBuffer.at(readIndex0);
    const float read1 = delayBuffer.at(readIndex1);
    *outSample = read0 * delayFrac0 + read1 * delayFrac1;

    // Update persistent write index
    ++writeIndex; writeIndex %= delayBufferSize;
}

void DelayLine::processBlock(float* outBlock, const float* inBlock, uint32_t numSamples, const float* modInput /*= nullptr*/)
{
    for (uint32_t n = 0; n < numSamples; n++)
        DelayLine::processSample(&outBlock[n], &inBlock[n], modInput ? modInput[n] : 0.0f);
}

}
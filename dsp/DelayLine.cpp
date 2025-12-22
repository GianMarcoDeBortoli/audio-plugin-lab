#include "DelayLine.h"

namespace DSP
{

DelayLine::DelayLine(size_t maxDelaySamples, size_t initDelaySamples) :
    delayRamp { 1200u } // Initialize the delay ramp with a default ramp time
{
    // Check if the maximum delay samples is valid
    jassert(maxDelaySamples > 0 && "Maximum delay of the delay line must be greater than zero");
    // Add 1 sample to the buffer size to effectively use the requested maximum delay
    delayBufferSize = maxDelaySamples + size_t { 1u };

    // Initialize the delay buffer with maximum delay size and fill it with zeros
    delayBuffer.resize(delayBufferSize, 0.f);

    // Initialize the current delay samples to the requested value
    jassert(initDelaySamples > 0 && "Initial delay of the delay line must be greater than zero");
    jassert(initDelaySamples <= maxDelaySamples && "Initial delay must be less than the maximum delay");
    currentDelaySamples = initDelaySamples;
    delayRamp.setTarget(static_cast<float>(initDelaySamples), true);

    // Initialize the current write index to zero
    currentWriteIndex = 0u;    
}

DelayLine::~DelayLine()
{
}

void DelayLine::setDelay(size_t newDelaySamples)
{
    jassert(newDelaySamples <= delayBufferSize - size_t{ 1u } && "New delay must be less than the maximum delay");
    currentDelaySamples = newDelaySamples;
    delayRamp.setTarget(static_cast<float>(newDelaySamples), true);
}

void DelayLine::prepare(double newSampleRate, int samplesPerBlock)
{
    jassert(newSampleRate > 0.0 && "Sample rate must be greater than zero");
    // Prepare the delay ramp with the new sample rate
    delayRamp.prepare(newSampleRate, samplesPerBlock);
}

void DelayLine::clear()
{
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.f);
    currentWriteIndex = 0u;
}

void DelayLine::processSample(float* outSample, const float* inSample)
{
    // Define write and read indices based on the current write index and delay samples
    const size_t writeIndex { currentWriteIndex };

    // Interpolate the read index for smooth ramping
    float delay;
    delayRamp.assignSample(&delay);
    const float delayCeil  { std::ceil(delay) };
    const float delayFrac1 { delayCeil - delay };
    const float delayFrac0 {  1.f - delayFrac1 };

    // Calculate interpolation read indices based on the modulation value
    const size_t readIndex0 { (writeIndex + delayBufferSize - static_cast<size_t>(delayCeil)) % delayBufferSize };
    const size_t readIndex1 { (readIndex0 + delayBufferSize + static_cast<size_t>(    1u   )) % delayBufferSize };

    // Write input to the delay buffer
    delayBuffer[writeIndex] = *inSample;
    // Read output from the delay buffer
    const float read0 = delayBuffer[readIndex0];
    const float read1 = delayBuffer[readIndex1];
    *outSample = read0 * delayFrac0 + read1 * delayFrac1;

    // Update persistent write index
    ++currentWriteIndex; currentWriteIndex %= delayBufferSize;
}

void DelayLine::processSample(float* outSample, const float* inSample, const float* modInput)
{
    // Define write and read indices based on the current write index and delay samples
    const size_t writeIndex { currentWriteIndex };

    // Interpolate the read index for smooth ramping
    float delay;
    delayRamp.assignSample(&delay);
    delay += *modInput;
    const float delayCeil { std::ceil(delay) };
    const float delayFrac1 { delayCeil - delay };
    const float delayFrac0 {  1.f - delayFrac1 };

    // Calculate interpolation read indices based on the modulation value
    const size_t readIndex0 { (writeIndex + delayBufferSize - static_cast<size_t>(delayCeil)) % delayBufferSize };
    const size_t readIndex1 { (readIndex0 + delayBufferSize + static_cast<size_t>(    1u   )) % delayBufferSize };

    // Write input to the delay buffer
    delayBuffer[writeIndex] = *inSample;
    // Read output from the delay buffer
    const float read0 = delayBuffer[readIndex0];
    const float read1 = delayBuffer[readIndex1];
    *outSample = read0 * delayFrac0 + read1 * delayFrac1;

    // Update persistent write index
    ++currentWriteIndex; currentWriteIndex %= delayBufferSize;
}

}
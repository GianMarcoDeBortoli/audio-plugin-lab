#include <cmath>
#include <cassert>

#include "DelayLine.h"

namespace apl
{
    namespace operators
{

DelayLine::DelayLine(size_t maxDelayLength, float initDelayLength) :
delayLength { initDelayLength }
{
    // Validity checks
    assert( maxDelayLength > 0 && "Maximum delay length must be greater than zero" );
    assert( initDelayLength >= 0.f && "Initial delay length must be greater than or equal to zero" );
    assert( initDelayLength <= static_cast<float>(maxDelayLength) && "Initial delay length must be less than the maximum length" );

    // Initialize the current delay with a smoothing time to the requested value
    delayLength.setSmoothingInterval(uint32_t { 1200u });

    // Initialize the delay buffer with maximum delay size and fill it with zeros
    delayBuffer.resize(maxDelayLength, 0.f);

    // Initialize the pointer and the flag
    pointer = size_t { 0u };
    hasRead = false;
}

//================================================
// SET METHODS
void DelayLine::setDelayLength(float newDelayLength)
{
    assert( newDelayLength >= 0.f && "New delay length must be greater than zero" );
    const auto bufferSize = delayBuffer.size();
    assert( newDelayLength <= static_cast<float>(bufferSize) && "New delay length must be less than the maximum length" );
    delayLength.setTarget(newDelayLength, false);
}

// GET METHODS
float DelayLine::getDelayLength() const
{
    return delayLength.getCurrentValue();
}

// STATE METHODS
void DelayLine::clear()
{
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.f);
}

void DelayLine::prepare()
{
    delayLength.prepare();
    DelayLine::clear();
    pointer = size_t { 0u };
    hasRead = false;
}

//================================================
// PROCESS METHODS
float DelayLine::readFromBuffer(float delay) const // private
{
    // Get buffer size
    const auto bufferSize = delayBuffer.size();

    // Interpolation ratios
    const float delayFloor { std::floor(delay) };
    const float delayFracR {  delay - delayFloor  };
    const float delayFracL {   1.f  - delayFracR  };

    // Pointers
    const size_t readPointerL { (   pointer    + bufferSize - static_cast<size_t>(delayFloor)) % bufferSize };
    const size_t readPointerR { ( readPointerL + bufferSize + static_cast<size_t>(    1u    )) % bufferSize };

    // Contributions
    const float sampleL = delayBuffer[readPointerL];
    const float sampleR = delayBuffer[readPointerR];

    // Interpolate samples
    return delayFracL * sampleL + delayFracR * sampleR;
}

void DelayLine::writeToBuffer(float sample) // private
{
    delayBuffer[pointer] = sample;
}

float DelayLine::getSample()
{
    // Check that we have not read during the current process routine already
    assert( hasRead == false );

    // Smooth current delay length towards target delay length
    const float delay = delayLength.getNextValue();
    assert( delay >= 1.f && "Delay length inside a feedback loop must be at least one sample");

    // Add modulation
    // if (modSource != nullptr)
    //     delay += modSource->getNextValue();
    // assert( delay >= 1.f && "Modulation depth is too large, delay became lower than 1.f");
    // assert( delay <= static_cast<float>(bufferSize) && "Modulation depth is too large, delay exceeded buffer size");
    
    // Read from the buffer
    float outSample = DelayLine::readFromBuffer(delay);
    // set flat to true
    hasRead = true;
    
    return outSample;
}

void DelayLine::setSample(const float inSample)
{
    // Check that we have already read during the current process routine
    assert( hasRead == true );

    // Write
    DelayLine::writeToBuffer(inSample);
}

void DelayLine::advancePointer()
{   
    const auto bufferSize = delayBuffer.size();
    // Update pointer
    ++pointer;
    pointer %= bufferSize;
    // Reset flag
    hasRead = false;
}

float DelayLine::processSample(const float inSample)
{
    const float delay = delayLength.getNextValue();

    // Add modulation
    // if (modSource != nullptr)
    //     delay += modSource->getNextValue();
    // assert( delay >= 0.f && "Modulation depth is too large, delay became negative");
    // assert( delay <= static_cast<float>(bufferSize) && "Modulation depth is too large, delay exceeded buffer size");

    float outSample;

    if (delay < 1.0f)
        outSample = inSample;
    else
        outSample = DelayLine::readFromBuffer(delay);

    DelayLine::writeToBuffer(inSample);
    advancePointer();

    return outSample;
}

void DelayLine::processBlock(float* outBlock, const float* inBlock, uint32_t numSamples)
{
    const auto bufferSize = delayBuffer.size();
    // Check feasibility
    assert( static_cast<size_t>(numSamples) <= bufferSize );

    // Process one sample at a time
    for (uint32_t n = 0; n < numSamples; n++)
        outBlock[n] = DelayLine::processSample(inBlock[n]);
}

}
}
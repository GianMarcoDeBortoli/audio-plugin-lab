#include <cmath>
#include <cassert>
#include <stdexcept>

#include "DelayLine.h"

namespace apl::operators
{

DelayLine::DelayLine(size_t maxDelayLength, float initDelayLength) :
delayLength { initDelayLength }
{
    // Validity checks
    if (maxDelayLength == 0)
        throw std::invalid_argument("Maximum delay length must be greater than zero");
    if (initDelayLength < 0.f)
        throw std::invalid_argument("Initial delay length must be greater than or equal to zero");
    if (initDelayLength > static_cast<float>(maxDelayLength))
        throw std::invalid_argument("Initial delay length must be less than the maximum length");

    // Initialize the current delay with a smoothing time to the requested value
    delayLength.setSmoothingInterval(uint32_t { 1200u });

    // Initialize the delay buffer with maximum delay size and fill it with zeros
    delayBuffer.resize(maxDelayLength, 0.f);

    // Initialize the pointer and the flag
    pointer = size_t { 0u };
    hasRead = false;
    hasWritten = false;
}

//================================================
// SET METHODS
void DelayLine::setDelayLength(float newDelayLength)
{
    const auto bufferSize = delayBuffer.size();
    
    if (newDelayLength < 0.f)
        throw std::invalid_argument("New delay length must be greater than or equal to zero");
    if (newDelayLength > static_cast<float>(bufferSize))
        throw std::invalid_argument("New delay length must be less than the maximum length");
    
    delayLength.setTarget(newDelayLength, false);
}

// GET METHODS
float DelayLine::getDelayLength() const noexcept
{
    return delayLength.getCurrentValue();
}

// STATE METHODS
void DelayLine::clear() noexcept
{
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.f);
}

void DelayLine::prepare() noexcept
{
    delayLength.prepare();
    DelayLine::clear();
    pointer = size_t { 0u };
    hasRead = false;
    hasWritten = false;
}

//================================================
// PROCESS METHODS
float DelayLine::readFromBuffer(float delay) const noexcept // private
{
    // Get buffer size
    const auto bufferSize = delayBuffer.size();

    // Interpolation ratios
    const float delayFloor { std::floor(delay) };
    const float delayFracL {  delay - delayFloor  };
    const float delayFracR {   1.f  - delayFracL  };

    // Pointers
    const size_t readPointerFloor {
        ( pointer + bufferSize - static_cast<size_t>(delayFloor))
        % bufferSize
    };

    const size_t readPointerCeil {
        ( readPointerFloor + bufferSize - 1u)
        % bufferSize
    };

    const float sampleFloor = delayBuffer[readPointerFloor];
    const float sampleCeil = delayBuffer[readPointerCeil];

    const float weightFloor = 1.f - (delay - delayFloor);
    const float weightCeil = delay - delayFloor;

    return weightFloor * sampleFloor + weightCeil * sampleCeil;
}

void DelayLine::writeToBuffer(float sample) noexcept // private
{
    delayBuffer[pointer] = sample;
}

void DelayLine::advance() noexcept
{
    const auto bufferSize = delayBuffer.size();
    // Update pointer
    ++pointer;
    pointer %= bufferSize;
}

float DelayLine::getSample()
{
    if (hasRead != false)
        throw std::invalid_argument("DelayLine::advancePointer must be called before reading another sample");

    assert (hasWritten == false
        && "set-get-advance routine has gone wrong" );

    // Smooth current delay length towards target delay length
    const float delay = delayLength.getNextValue();
    assert( delay >= 1.f
        && "Delay length inside a feedback loop must be at least one sample" );

    // Add modulation
    // if (modSource != nullptr)
    //     delay += modSource->getNextValue();
    // if (delay < 1.f)
    //     throw std::invalid_argument("Modulation depth is too large, delay became lower than 1.f");
    // if (delay > static_cast<float>(bufferSize)
    //     throw atd::invalid_argument("Modulation depth is too large, delay exceeded buffer size");
    
    // Read from the buffer
    float outSample = DelayLine::readFromBuffer(delay);
    // set flat to true
    hasRead = true;
    
    return outSample;
}

void DelayLine::setSample(float inSample)
{
    if (hasRead == false)
        throw std::invalid_argument("DelayLine::getSample must be called before DelayLine::setSample");
    if (hasWritten != false)
        throw std::invalid_argument("DelayLine::advancePointer must be called before writing another sample");

    DelayLine::writeToBuffer(inSample);

    hasWritten = true;
}

void DelayLine::advancePointer()
{   
    if (hasRead == false)
        throw std::invalid_argument("DelayLine::getSample must be called before DelayLine::advancePointer");
    if (hasWritten == false)
        throw std::invalid_argument("DelayLine::setSample must be called before DelayLine::advancePointer");

    // Advance the pointer
    DelayLine::advance();

    // Reset flags
    hasRead = false;
    hasWritten = false;
}

float DelayLine::processSample(float inSample) noexcept
{
    const float delay = delayLength.getNextValue();

    // Add modulation
    // if (modSource != nullptr)
    //     delay += modSource->getNextValue();
    // assert( delay >= 0.f
    //     && "Modulation depth is too large, delay became negative");
    // assert( delay <= static_cast<float>(bufferSize)
    //     && "Modulation depth is too large, delay exceeded buffer size");

    float outSample;

    if (delay < 1.0f)
        outSample = inSample;
    else
        outSample = DelayLine::readFromBuffer(delay);

    DelayLine::writeToBuffer(inSample);
    DelayLine::advance();

    return outSample;
}

void DelayLine::processBlock(float* outBlock, const float* inBlock, uint32_t numSamples) noexcept
{
    const auto bufferSize = delayBuffer.size();
    // Check feasibility
    assert( static_cast<size_t>(numSamples) <= bufferSize );

    for (uint32_t n = 0; n < numSamples; ++n)
        outBlock[n] = DelayLine::processSample(inBlock[n]);
}

}
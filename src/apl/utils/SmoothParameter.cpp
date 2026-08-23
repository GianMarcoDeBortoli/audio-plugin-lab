#include <cmath>
#include <cassert>

#include "SmoothParameter.h"

namespace apl
{
    namespace utils
{

SmoothParameter::SmoothParameter(float initValue) :
    currentValue { initValue },
    targetValue { initValue },
    stepSize { 0.0f },
    smoothingInterval { defaultSmoothingInterval }
{}

//================================================
// SET METHODS
void SmoothParameter::setSmoothingInterval(uint32_t newSmoothingInterval)
{
    assert(newSmoothingInterval > 0);
    smoothingInterval = newSmoothingInterval;
}

void SmoothParameter::setTarget(float newTargetValue, bool skipSmoothing)
{
    targetValue = newTargetValue;

    if (skipSmoothing || (std::abs(newTargetValue - currentValue) <= minDelta))
    {
        currentValue = targetValue;
        stepSize = 0.0;
        return;
    }

    stepSize = (targetValue - currentValue) / static_cast<float>(smoothingInterval);
}

// GET METHODS
uint32_t SmoothParameter::getSmoothingInterval() const
{
    return smoothingInterval;
}

float SmoothParameter::getTarget() const
{
    return targetValue;
}

float SmoothParameter::getCurrentValue() const
{
    return currentValue;
}

// STATE METHODS
bool SmoothParameter::isSmoothing() const
{
    const float targetDelta { std::fabs(targetValue - currentValue) };
    return ((targetDelta > std::fabs(2.f * stepSize)) && (std::fabs(stepSize) > minDelta));
}

void SmoothParameter::update() // private
{
    if (SmoothParameter::isSmoothing())
        currentValue += stepSize;
}

void SmoothParameter::prepare()
{
    currentValue = targetValue;
    stepSize = 0.0;
}

//================================================
// PROCESSING METHODS

float SmoothParameter::getNextValue()
{
    SmoothParameter::update();
    return currentValue;
}

void SmoothParameter::getNextValues(float* block, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        block[n] = SmoothParameter::getNextValue();
    }
}

}
}
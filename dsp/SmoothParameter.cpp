// #include <algorithm> 
#include <cmath>
#include <cassert>

#include "SmoothParameter.h"

namespace utils
{

SmoothParameter::SmoothParameter(float initValue) :
    currentValue { initValue },
    targetValue { initValue },
    smoothingSamples { minSmoothingSamples },
    smoothingStep { 0.0f }
{}

//================================================

float SmoothParameter::getCurrentValue()
{
    return currentValue;
}

void SmoothParameter::setSmoothingTime(uint32_t newTimeInSamples)
{
    assert(newTimeInSamples >= minSmoothingSamples);
    smoothingSamples = newTimeInSamples;
}

uint32_t SmoothParameter::getSmoothingTime()
{
    return smoothingSamples;
}

void SmoothParameter::setTarget(float newTargetValue, bool skipSmoothing)
{
    if (std::abs(newTargetValue - currentValue) > minDelta)
    {
        targetValue = newTargetValue;
        smoothingStep = (targetValue - currentValue) / static_cast<float>(smoothingSamples);
    }

    if (skipSmoothing)
        currentValue = targetValue = newTargetValue;
}

float SmoothParameter::getTarget()
{
    return targetValue;
}

bool SmoothParameter::needsSmoothing()
{
    const float targetDelta { std::fabs(targetValue - currentValue) };
    return ((targetDelta > std::fabs(2.f * smoothingStep)) && (std::fabs(smoothingStep) > minDelta));
}

//================================================

void SmoothParameter::prepare()
{
    SmoothParameter::setTarget(targetValue, true);
}

//================================================

void SmoothParameter::update()
{
    if (SmoothParameter::needsSmoothing())
        currentValue += smoothingStep;
    else
        currentValue = targetValue;
}

float SmoothParameter::getSample()
{
    SmoothParameter::update();
    return currentValue;
}

void SmoothParameter::getBlock(float* block, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        block[n] = SmoothParameter::getSample();
    }
}

}
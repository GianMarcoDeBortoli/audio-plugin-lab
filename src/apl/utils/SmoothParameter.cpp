#include <cmath>
#include <stdexcept>

#include "SmoothParameter.h"

namespace apl::utils
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
    if (newSmoothingInterval == 0)
        throw std::invalid_argument("Smoothing interval must be positive" );
    smoothingInterval = newSmoothingInterval;
}

void SmoothParameter::setTarget(float newTargetValue, bool skipSmoothing) noexcept
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
uint32_t SmoothParameter::getSmoothingInterval() const noexcept
{
    return smoothingInterval;
}

float SmoothParameter::getTarget() const noexcept
{
    return targetValue;
}

float SmoothParameter::getCurrentValue() const noexcept
{
    return currentValue;
}

// STATE METHODS
bool SmoothParameter::isSmoothing() const noexcept
{
    return std::fabs(targetValue - currentValue) > minDelta
        && std::fabs(stepSize) > minDelta;
}

void SmoothParameter::update() noexcept // private
{
    if (!isSmoothing())
        return;

    const float nextValue = currentValue + stepSize;

    // Reached or crossed the target.
    if ((stepSize > 0.0f && nextValue >= targetValue) ||
        (stepSize < 0.0f && nextValue <= targetValue))
    {
        currentValue = targetValue;
        stepSize = 0.0f;
        return;
    }

    currentValue = nextValue;
}

void SmoothParameter::prepare() noexcept
{
    currentValue = targetValue;
    stepSize = 0.0;
}

//================================================
// PROCESS METHODS

float SmoothParameter::getNextValue() noexcept
{
    SmoothParameter::update();
    return currentValue;
}

void SmoothParameter::getNextValues(float* block, uint32_t numSamples) noexcept
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        block[n] = SmoothParameter::getNextValue();
    }
}

}
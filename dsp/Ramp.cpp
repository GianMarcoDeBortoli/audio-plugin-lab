#include "Ramp.h"

namespace DSP
{
// Constructors
Ramp::Ramp(uint32_t rampTime) :
    rampTimeSamples { std::max(rampTime, minRampSamples) }
{}

Ramp::~Ramp()
{}

void Ramp::setRampTime(uint32_t newRampTimeSamples)
{
    rampTimeSamples = std::max(newRampTimeSamples, minRampSamples);
}

void Ramp::setTarget(float newTargetValue, bool skipRamp)
{
    if (std::abs(newTargetValue - currentValue) > minDelta)
    {
        targetValue = newTargetValue;
        rampStep = (targetValue - currentValue) / static_cast<float>(rampTimeSamples);
    }

    if (skipRamp)
        currentValue = targetValue = newTargetValue;
}

void Ramp::prepare(double newSampleRate, int samplesPerBlock)
{
    jassert(newSampleRate > 0.0 && "Sample rate must be greater than zero");
    uint32_t newRampTimeSamples = static_cast<uint32_t>(std::round(rampTimeSamples * (newSampleRate / sampleRate)));
    rampTimeSamples = std::max(newRampTimeSamples, minRampSamples);
    sampleRate = newSampleRate;
}

void Ramp::sumSample(float* outSample, const float* inSample)
{
    const float targetDelta { std::fabs(targetValue - currentValue) };
    if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
        currentValue += rampStep;
    else
        currentValue = targetValue;

    *outSample = currentValue + *inSample;
}

void Ramp::sumBuffer(float* outBuffer, const float* inBuffer, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        const float targetDelta { std::fabs(targetValue - currentValue) };
        if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
            currentValue += rampStep;
        else
            currentValue = targetValue;

        outBuffer[n] = currentValue + inBuffer[n];
    }
}

void Ramp::sumBuffer(float* const* outBuffer, const float* const* inBuffer, uint32_t numChannels, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        const float targetDelta { std::fabs(targetValue - currentValue) };
        if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
            currentValue += rampStep;
        else
            currentValue = targetValue;

        for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        {
            outBuffer[ch][n] = currentValue + inBuffer[ch][n];
        }
    }
}

void Ramp::multiplySample(float* outSample, const float* inSample)
{
    const float targetDelta{ std::fabs(targetValue - currentValue) };
    if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
        currentValue += rampStep;
    else
        currentValue = targetValue;

    *outSample = currentValue * *inSample;
}

void Ramp::multiplyBuffer(float* outBuffer, const float* inBuffer, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        const float targetDelta { std::fabs(targetValue - currentValue) };
        if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
            currentValue += rampStep;
        else
            currentValue = targetValue;

        outBuffer[n] = currentValue * inBuffer[n];
    }
}

void Ramp::multiplyBuffer(float* const* outBuffer, const float* const* inBuffer, uint32_t numChannels, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        const float targetDelta { std::fabs(targetValue - currentValue) };
        if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
            currentValue += rampStep;
        else
            currentValue = targetValue;

        for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        {
            outBuffer[ch][n] = currentValue * inBuffer[ch][n];
        }
    }
}

void Ramp::assignSample(float* sample)
{
    const float targetDelta { std::fabs(targetValue - currentValue) };
    if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
        currentValue += rampStep;
    else
        currentValue = targetValue;

    *sample = currentValue;
}

void Ramp::assignBuffer(float* buffer, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        const float targetDelta { std::fabs(targetValue - currentValue) };
        if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
            currentValue += rampStep;
        else
            currentValue = targetValue;

        buffer[n] = currentValue;
    }
}

void Ramp::assignBuffer(float* const* buffer, uint32_t numChannels, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        const float targetDelta { std::fabs(targetValue - currentValue) };
        if ((targetDelta > std::fabs(2.f * rampStep)) && (std::fabs(rampStep) > minDelta))
            currentValue += rampStep;
        else
            currentValue = targetValue;

        for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
        {
            buffer[ch][n] = currentValue;
        }
    }
}

}
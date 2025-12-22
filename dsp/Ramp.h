#pragma once

#include <JuceHeader.h>

namespace DSP
{

class Ramp
{
public:
    // Constructors
    Ramp(uint32_t initRampTimeSamples);
    // Destructor
    ~Ramp();

    // No copy semantics
    Ramp(const Ramp&) = delete;
    const Ramp& operator=(const Ramp&) = delete;

    // No move semantics
    Ramp(Ramp&&) noexcept = default;
    Ramp& operator=(Ramp&&) noexcept = default;

    // =============================================

    // Set new ramp time
    void setRampTime(uint32_t newRampTimeSamples);

    // Set the target value. Allows to skip the ramp
    void setTarget(float newTargetValue, bool skipRamp = false);

    // =============================================

    // Update the sample rate. Allows to skip to ramp value
    void prepare(double newSampleRate, int samplesPerBlock);

    // The ramp value is summed to the input
    // Process single-channel sample
    void sumSample(float* outSample, const float* inSample);
    // Process single-channel buffer
    void sumBuffer(float* outBuffer, const float* inBuffer, uint32_t numSamples);
    // Process multi-channel buffer
    void sumBuffer(float* const* outBuffer, const float* const* inBuffer, uint32_t numChannels, uint32_t numSamples);

    // The ramp is multiplied to the input
    // Process single-channel sample
    void multiplySample(float* outSample, const float* inSample);
    // Process single-channel buffer
    void multiplyBuffer(float* outBuffer, const float* inBuffer, uint32_t numSamples);
    // Process multi-channel buffer
    void multiplyBuffer(float* const* outBuffer, const float* const* inBuffer, uint32_t numChannels, uint32_t numSamples);

    // Assign the ramp value to the input
    // Process single-channel sample
    void assignSample(float* sample);
    // Process single-channel buffer
    void assignBuffer(float* buffer, uint32_t numSamples);
    // Process multi-channel buffer
    void assignBuffer(float* const* buffer, uint32_t numChannels, uint32_t numSamples);

    // =============================================

    // Minimum ramp time in seconds ( 1ms)
    static constexpr uint32_t minRampSamples { 48u };  // 1ms at 48kHz
    // Minimum absolute difference between target and current value
    static constexpr float minDelta { 1e-9f };

private:
    double sampleRate { 48000.0 };

    uint32_t rampTimeSamples;
    float rampStep { 0.f };
    float targetValue { 0.f };
    float currentValue { 0.f };

    // static_assert(std::is_copy_constructible_v<Ramp>);
    // static_assert(std::is_move_constructible_v<Ramp>);
    static_assert(std::is_nothrow_move_assignable_v<Ramp>);
};

}
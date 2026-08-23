#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "utils/SmoothParameter.h"
// #include "Waveform.h"

namespace apl
{
    namespace operators
{

class DelayLine
{
public:

    // Constructor
    DelayLine() = delete;
    DelayLine(
        size_t maxDelayLength,
        float initDelayLength
    );

    // Destructor -> default

    // Copy
    DelayLine(const DelayLine&) = delete;
    DelayLine& operator=(const DelayLine&) = delete;

    // Move
    DelayLine(DelayLine&&) noexcept = default;
    DelayLine& operator=(DelayLine&&) noexcept = default;

    //================================================
    // SET METHODS
    // Set the current delay length
    void setDelayLength(float newDelayLength);

    // GET METHODS
    // Get the current delay length
    float getDelayLength() const;

    // STATE METHODS
    // Clear the content of the delay buffer
    void clear();
    // Prepare the delay line for processing
    void prepare();

    //================================================
    // PROCESS METHODS
    // Get the sample at the current delay value
    float getSample();
    // Set the sample at the current write pointer
    void setSample(const float inSample);
    // Advance write pointer
    void advancePointer();
    
    // Process audio sample - linear interpolation only
    float processSample(const float inSample);

    // Process block of audio samples - wrapper of processSample
    void processBlock(float* outBlock, const float* inBlock, uint32_t numSamples);

    //================================================

private:

    // Utils
    float readFromBuffer(float delay) const;
    void writeToBuffer(float sample);

    // generator::WaveformSource &modSource;
    apl::utils::SmoothParameter delayLength;
    std::vector<float> delayBuffer;
    size_t pointer;
    bool hasRead;
};

static_assert(std::is_move_constructible_v<DelayLine>, "DelayLine must be movable");
static_assert(std::is_nothrow_move_assignable_v<DelayLine>, "Move assignment should not throw");

}
}
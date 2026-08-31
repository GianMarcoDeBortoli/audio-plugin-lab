#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "utils/SmoothParameter.h"
// #include "Waveform.h"

namespace apl::operators
{

class DelayLine
{

/*
DelayLine can be use in a forward only architecture through the methods
DelayLine::processSample or DelayLine::processBlock
and it can be used inside a feedback loop through the methods
DelayLine::getSample, DelayLine::setSample, DelayLine::advancePointer
*/

public:

    // Constructor
    DelayLine() = delete;
    DelayLine(
        size_t maxDelayLength,
        float initDelayLength
    );

    // Destructor
    ~DelayLine() = default;

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
    float getDelayLength() const noexcept;

    //================================================
    // STATE METHODS
    // Clear the content of the delay buffer
    void clear() noexcept;
    // Prepare the delay line for processing
    void prepare() noexcept;

    //================================================
    // PROCESS METHODS
    // Get the sample at the current delay value
    float getSample();
    // Set the sample at the current write pointer
    void setSample(float inSample);
    // Advance write pointer
    void advancePointer();
    
    // Process audio sample - single channel - linear interpolation only
    float processSample(float inSample) noexcept;

    // Process audio block - wrapper of processSample
    void processBlock(float* outBlock, const float* inBlock, uint32_t numSamples) noexcept;

    //================================================

private:

    float readFromBuffer(float delay) const noexcept;
    void writeToBuffer(float sample) noexcept;
    void advance() noexcept;

    //================================================

    // generator::WaveformSource &modSource;
    apl::utils::SmoothParameter delayLength;
    std::vector<float> delayBuffer;
    size_t pointer;
    bool hasRead;
    bool hasWritten;

};

static_assert(!std::is_copy_constructible_v<DelayLine>,
    "DelayLine must not be copyable");
static_assert(!std::is_copy_assignable_v<DelayLine>,
    "DelayLine must not be copy assignable");
static_assert(std::is_nothrow_move_constructible_v<DelayLine>,
    "DelayLine move construction should not throw");
static_assert(std::is_move_constructible_v<DelayLine>,
    "DelayLine must be movable");
static_assert(std::is_nothrow_move_assignable_v<DelayLine>,
    "DelayLine move assignment should not throw");

}
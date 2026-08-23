#pragma once

// #include <cstdint>
// #include <cstddef>
// #include <type_traits>

#include "SmoothParameter.h"

namespace generator
{

class Waveform
{
public:
    // Constructor
    Waveform() = delete;
    explicit Waveform(float initFrequency);

    // Destructor -> default

    // Copy
    Waveform(const Waveform&) = default;
    Waveform& operator=(const Waveform&) = default;

    // Move
    Waveform(Waveform&&) = default;
    Waveform& operator=(Waveform&&) = default;

    //================================================
    
    void setFrequency(float newFrequency);

    //================================================

    void prepare();

    //================================================

    void fadeOut();
    void fadeIn();

    //================================================

    virtual void processSample(float* outSample, float* inSample);

    void processBlock(float* outBlock, float* inBlock, uint32_t numSamples);

private:

    float frequency;

    utils::SmoothParameter amplitude { float { 1.0f } };

};

}
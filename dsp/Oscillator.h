#pragma once

// #include <cstdint>
// #include <cstddef>
#include <type_traits>

#include "SmoothParameter.h"

namespace primitives
{

class Oscillator
{
public:
    // Constructor
    Oscillator() = delete;
    explicit Oscillator(float initFrequency);

    // Destructor -> default

    // Copy
    Oscillator(const Oscillator&) = default;
    Oscillator& operator=(const Oscillator&) = default;

    // Move
    Oscillator(Oscillator&&) = default;
    Oscillator& operator=(Oscillator&&) = default;

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
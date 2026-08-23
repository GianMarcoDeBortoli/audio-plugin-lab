#pragma once

// #include <cstdint>
// #include <cstddef>
#include <type_traits>

#include "SmoothParameter.h"

namespace primitives
{

class Sinewave
{
public:
    // Constructor
    Sinewave() = delete;
    explicit Sinewave(float initFrequency);

    // Destructor -> default

    // Copy
    Sinewave(const Sinewave&) = default;
    Sinewave& operator=(const Sinewave&) = default;

    // Move
    Sinewave(Sinewave&&) = default;
    Sinewave& operator=(Sinewave&&) = default;

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
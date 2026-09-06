#pragma once

#include <cstddef>
#include <random>
#include <vector>

#include "math/constants.h"
#include "dsp/FFT.h"
#include "utils/SmoothParameter.h"

namespace apl::operators
{

class TimeVaryingMatrix
{
public:

    // Constructor
    TimeVaryingMatrix() = delete;
    TimeVaryingMatrix(
        size_t dimension,
        float initCyclesPerSample,
        float initDepth,
        float initSpread,
        std::mt19937& rng
    );

    // Destuctor -> default

    // Copy
    TimeVaryingMatrix(const TimeVaryingMatrix&) = delete;
    const TimeVaryingMatrix& operator=(const TimeVaryingMatrix&) = delete;

    // Move
    TimeVaryingMatrix(TimeVaryingMatrix&&) noexcept = default;
    TimeVaryingMatrix& operator=(TimeVaryingMatrix&&) noexcept = delete;

    // =============================================
    // SET METHODS
    void setCyclesPerSample(float newCyclesPerSample);
    void setDepth(float newDepth);
    void setSpread(float newSpread);

    // STATE METHODS
    void reset();
    void prepare();

    // =============================================
    // PROCESS METHODS
    // Process audio sample - multichannel
    void processSample(
        float* output,
        const float* input
    );

    //==============================================

private:

    void setModulationParameters() noexcept;

    //==============================================
    static constexpr float pi = apl::math::constants::pi;

    // High-level parameters
    size_t dimension;
    apl::utils::SmoothParameter baseCyclesPerSample;
    apl::utils::SmoothParameter baseDepth;
    apl::utils::SmoothParameter baseSpread;
    size_t sampleIndex{0};

    // Low-level parameters
    std::mt19937& rng;
    // Initialize distribution
    std::uniform_real_distribution<float> phaseDist;
    std::uniform_real_distribution<float> spreadDist;
    size_t numOscillators;
    std::vector<float> phaseValues;
    std::vector<float> freqValues;
    std::vector<float> depthValues;

    // FFT engine
    apl::dsp::FFT fft;
    std::vector<float> fftBuffer;

};

// static_assert(std::is_copy_constructible_v<Matrix>);
// static_assert(std::is_move_constructible_v<Matrix>);
// static_assert(std::is_nothrow_move_assignable_v<TimeVaryingMatrix>);
}
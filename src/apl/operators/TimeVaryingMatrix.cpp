#include <cassert>
#include <cmath>
#include <random>

#include "operators/TimeVaryingMatrix.h"

namespace apl::operators
{

size_t getFFTOrder(size_t dimension)
{
    if (dimension == 0 || (dimension & (dimension - 1)) != 0)
        throw std::invalid_argument(
            "TimeVaryingMatrix dimension must be a positive power of two"
        );

    size_t order = 0;
    while (dimension > 1)
    {
        dimension >>= 1;
        ++order;
    }

    return order;
}

TimeVaryingMatrix::TimeVaryingMatrix(
    size_t dimension,
    float initCyclesPerSample,
    float initDepth,
    float initSpread,
    std::mt19937& rng
) : 
    dimension { dimension },
    baseCyclesPerSample { initCyclesPerSample },
    baseDepth { initDepth },
    baseSpread { initSpread },
    rng { rng },
    phaseDist { std::uniform_real_distribution<float> { 0.f, 2.f * pi} },
    spreadDist { std::uniform_real_distribution<float> { -1.f, 1.f } },
    numOscillators { dimension / 2 - 1 },
    fft { getFFTOrder(dimension) }
{
    // Validate values
    if (initCyclesPerSample < 0.f || initCyclesPerSample >= 0.5f)
        throw std::invalid_argument("Modulation frequency in cycles per sample must be between 0 and 0.5");
    if (initDepth < 0.f || initDepth > 1.f)
        throw std::invalid_argument("Modulation depth must be between 0 and 1");
    if (initSpread < 0.f || initSpread > 1.f)
        throw std::invalid_argument("Modulation spread must be between 0 and 1");

    phaseValues.resize(numOscillators);
    freqValues.resize(numOscillators);
    depthValues.resize(numOscillators);

    // Draw from distributions
    TimeVaryingMatrix::setModulationParameters();

    fftBuffer.resize(2 * dimension);
}

// SET METHODS
void TimeVaryingMatrix::setCyclesPerSample(float newCyclesPerSample)
{
    if (newCyclesPerSample < 0.f || newCyclesPerSample >= 0.5f)
        throw std::invalid_argument("Modulation frequency in cycles per sample must be between 0 and 0.5");

    baseCyclesPerSample.setTarget(newCyclesPerSample);
    TimeVaryingMatrix::setModulationParameters();
}

void TimeVaryingMatrix::setDepth(float newDepth)
{
    if (newDepth < 0 || newDepth > 1)
        throw std::invalid_argument("Modulation depth must be between 0 and 1");

    baseDepth.setTarget(newDepth);
    TimeVaryingMatrix::setModulationParameters();
}

void TimeVaryingMatrix::setSpread(float newSpread)
{
    if (newSpread < 0 || newSpread > 1)
        throw std::invalid_argument("Modulation spread must be between 0 and 1");

    baseSpread.setTarget(newSpread);
    TimeVaryingMatrix::setModulationParameters();
}

void TimeVaryingMatrix::setModulationParameters() noexcept
{
    for (size_t i = 0; i < numOscillators; ++i)
    {
        phaseValues[i] = phaseDist(rng);
        freqValues[i] = baseCyclesPerSample.getNextValue() * (1.f + baseSpread.getNextValue() * spreadDist(rng));
        depthValues[i] = baseDepth.getNextValue() * (1.f + baseSpread.getNextValue() * spreadDist(rng));
    }
}

// STATE METHODS
void TimeVaryingMatrix::reset()
{
    sampleIndex = 0;
}

void TimeVaryingMatrix::prepare()
{
}

// PROCESS METHODS
void TimeVaryingMatrix::processSample(
    float* output,
    const float* input
)
{
    assert(output != nullptr);
    assert(input != nullptr);

    for (size_t i = 0; i < dimension; ++i)
        fftBuffer[i] = input[i];

    fft.rfft(fftBuffer.data(), fftBuffer.size());

    for (size_t k = 1; k < dimension / 2; ++k)
    {
        const size_t oscIndex = k - 1;

        const float angle =
            depthValues[oscIndex] *
            std::sin( 2.f * pi * freqValues[oscIndex] * sampleIndex
                + phaseValues[oscIndex] );

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        const float real = fft.getBinReal( fftBuffer.data(), fftBuffer.size(), k );
        const float imag = fft.getBinImag( fftBuffer.data(), fftBuffer.size(), k );

        const float newReal = c * real - s * imag;
        const float newImag = s * real + c * imag;

        fft.setBinValue( fftBuffer.data(), fftBuffer.size(), k, newReal, newImag );
    }

    fft.irfft(fftBuffer.data(), fftBuffer.size());

    for (size_t i = 0; i < dimension; ++i)
        output[i] = fftBuffer[i];


    ++sampleIndex;
}

}
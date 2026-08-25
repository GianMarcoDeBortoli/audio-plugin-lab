#include <cassert>
#include <cmath>
#include <random>

#include "operators/TimeVaryingMatrix.h"

namespace apl::operators
{

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
    numOscillators { dimension / 2 - 1 },
    fft { static_cast<size_t>(std::log2(dimension)) }
{
    // Validate values
    if (dimension == 0 || (dimension & (dimension - 1)) != 0)
        throw std::invalid_argument("TimeVaryingMatrix order must be a positive power of two");
    if (initDepth < 0 || initDepth > 1)
        throw std::invalid_argument("Modulation depth must be between 0 and 1");
    if (initSpread < 0 || initSpread > 1)
        throw std::invalid_argument("Modulation spread must be between 0 and 1");

    phaseValues.resize(numOscillators);
    freqValues.resize(numOscillators);
    depthValues.resize(numOscillators);

    // Initialize distribution
    std::uniform_real_distribution<float> phaseDist(0.f, 2.f * pi);
    std::uniform_real_distribution<float> spreadDist(-1.f, 1.f );

    // Draw from distributions
    for (size_t i = 0; i < numOscillators; ++i)
    {
        phaseValues[i] = phaseDist(rng);
        freqValues[i] = baseCyclesPerSample * (1.f + baseSpread * spreadDist(rng));
        depthValues[i] = baseDepth * (1.f + baseSpread * spreadDist(rng));
    }

    fftBuffer.resize(dimension);
}

void TimeVaryingMatrix::reset()
{
    sampleIndex = 0;
}

void TimeVaryingMatrix::prepare()
{
}


void TimeVaryingMatrix::processSample(
    float* output,
    const float* input
)
{
    assert(output != nullptr);
    assert(input != nullptr);

    for (size_t i = 0; i < dimension; ++i)
        fftBuffer[i] = input[i];

    fft.rfft(fftBuffer.data());

    for (size_t k = 1; k < dimension / 2; ++k)
    {
        const size_t oscIndex = k - 1;

        const float angle =
            depthValues[oscIndex] *
            std::sin( 2.f * pi * freqValues[oscIndex] * sampleIndex
                + phaseValues[oscIndex] );

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        const float real = fft.getBinReal( fftBuffer.data(), k );
        const float imag = fft.getBinImag( fftBuffer.data(), k );

        const float newReal = c * real - s * imag;
        const float newImag = s * real + c * imag;

        fft.setBinValue( fftBuffer.data(), k, newReal, newImag );
    }

    fft.irfft(fftBuffer.data());

    for (size_t i = 0; i < dimension; ++i)
        output[i] = fftBuffer[i];


    ++sampleIndex;
}

}
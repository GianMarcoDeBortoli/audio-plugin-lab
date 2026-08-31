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
    numOscillators { dimension / 2 - 1 },
    fft { getFFTOrder(dimension) }
{
    // Validate values
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

    fftBuffer.resize(2 * dimension);
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